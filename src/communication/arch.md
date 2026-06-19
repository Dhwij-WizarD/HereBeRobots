# Communication Package — Architecture

## Purpose

The `communication` package provides a ROS-agnostic abstraction over inter-process
communication. Business logic in other packages depends only on the concepts defined
here, not on rclcpp or any transport directly.

## Concept Hierarchy

All contracts are expressed as C++20 concepts in `Concepts.hpp`. Nothing uses
inheritance; concept satisfaction is verified at compile time via `static_assert`.

```
Publisher<T, Msg>         — single outbound channel (Publish)
Subscriber<T, Msg>        — single inbound channel (StartHandler)
Requester<T, Req, Res>    — single synchronous request
Responder<T, Req, Res>    — single synchronous response handler

MessengerApp<T>           — Login/Logout + dynamic pub/sub channels (any Msg type)
CommunicatorApp<T>        — MessengerApp + Setup + service clients + service servers
StreamerApp<T>            — Login/Logout + streaming channels (Streamer/Audience)
FullCommunicatorApp<T>    — CommunicatorApp && StreamerApp
CommunicatorInterface<T,M,C,FC,S>
                          — Install/Uninstall/Get for each of the four app categories
```

The proxy structs (`ProxyStreamerApp`, `ProxyFullCommunicatorApp`) exist solely to
make concept membership assertions readable without pulling in a real backend:

```cpp
static_assert(StreamerApp<ProxyStreamerApp>);
static_assert(FullCommunicatorApp<ProxyFullCommunicatorApp>);
```

## Communicator (the switchboard)

`Communicator` satisfies `CommunicatorInterface` and is the object every node owns.
It holds four typed maps — one per app category — each keyed by a string name:

```
messengers       : unordered_map<string, MessengerApp_VT>
communicators    : unordered_map<string, CommunicatorApp_VT>
fullCommunicators: unordered_map<string, FullCommunicatorApp_VT>
streamers        : unordered_map<string, StreamerApp_VT>
```

The value types (`MessengerApp_VT`, `CommunicatorApp_VT`, …) are `std::variant`s of
`unique_ptr<Backend>`. Wrapping in `unique_ptr` lets the variant hold non-movable
concrete types (like `RosCommunicatorApp`, which contains a `std::mutex`).

### Closed-set polymorphism

When a new transport backend is added (e.g., MQTT), its `unique_ptr` is added to the
relevant variant type aliases in `Communicator.hpp`. Dispatch is then compile-time
via `std::get_if`, with no virtual functions.

### Typical node setup

```cpp
Communicator comm;
comm.InstallCommunicatorApp<RosCommunicatorApp>("ros");

auto* app = comm.GetCommunicatorApp<RosCommunicatorApp>("ros");
app->Setup(argc, argv);   // rclcpp::init() — once per process
app->Login("my_node");

app->CreatePublisher<std_msgs::msg::Int32>("/topic");
app->CreateSubscriber<std_msgs::msg::Int32>("/topic", cb);
```

`Login` and `Logout` provide per-use-case node isolation. A node can log out, change
its ROS node name, and log back in without reinitializing the whole rclcpp context.

## RosCommunicatorApp

The one concrete `CommunicatorApp` backend. It wraps an `rclcpp::Node` and a
`MultiThreadedExecutor` and satisfies `CommunicatorApp<RosCommunicatorApp>`.

### Lifecycle

```
Setup(argc, argv)   → rclcpp::init()              (once per process)
Login(id)           → create Node + Executor, start spin thread
  …pub/sub/client/service operations…
Logout()            → cancel executor, join thread, teardown node
~RosCommunicatorApp → Logout() + rclcpp::shutdown()
```

Only one `RosCommunicatorApp` per process should call `Setup()`. The destructor
always calls `rclcpp::shutdown()`, which is idempotent (returns false if already
shut down), so teardown order across multiple instances is safe.

### Spin readiness

`MultiThreadedExecutor::spin()` is blocking and must run before `cancel()` can be
called safely. `Login()` solves this with a `std::promise<void> spinStarted`:

```
Login():                      Run() (background thread):
  t = std::thread{Run}  →       spinStarted.set_value()   ← unblocks Login
  started.wait()        ←       pExecutor->spin()
  return OK
```

`Logout()` calls `cancel()` only after `Login()` has returned, so the spin thread
is guaranteed to be inside `spin()` when cancel is requested.

### Channel storage

Each channel type is stored type-erased by name in its own map:

```
publishers  : unordered_map<string, PublisherBase::SharedPtr>
subscribers : unordered_map<string, SubscriptionBase::SharedPtr>
clients     : unordered_map<string, ClientBase::SharedPtr>
services    : unordered_map<string, ServiceBase::SharedPtr>
```

`GetPublisher<MSG>(name)` casts back to the typed shared pointer via
`static_pointer_cast`. Type safety is the caller's responsibility (same name,
same type used at creation and retrieval).

## Process-level invariants

- `rclcpp::init` / `rclcpp::shutdown` must be paired and sequential. Two consecutive
  inits or two consecutive shutdowns are not allowed.
- One `RosCommunicatorApp` instance owns the rclcpp context (called `Setup()`). All
  others in the same process share the already-initialized context.
- Per-test isolation in GTesting: a single global `Communicator` + `RosCommunicatorApp`
  is created in `main()`, and each test uses `Login`/`Logout` to get a fresh node.

## Adding a new backend

1. Implement the class and verify it satisfies the required concept via `static_assert`.
2. Add `std::unique_ptr<NewBackend>` to the relevant `*_VT` variant aliases in
   `Communicator.hpp`.
3. No changes to `Communicator` itself — `Install`/`Get` template methods work for
   any type already in the variant.
