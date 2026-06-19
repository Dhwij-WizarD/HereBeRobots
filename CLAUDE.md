# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

HereBeRobots (HBR) is a lightweight 2D SLAM and navigation stack for mobile robots built on ROS 2. The stack uses lidar, occupancy grids, and probabilistic state estimation. All C++ lives under the `HBR::` namespace hierarchy.

## Build System

The workspace uses `colcon` (ROS 2 build tool) for all ROS packages. Source the ROS 2 environment before building.

```bash
# Full workspace build (Release by default)
./Build.sh

# Debug build
./Build.sh Debug

# Clean then build
./Build.sh --clean
./Build.sh --clean Debug

# Source the install before running nodes
source install/setup.bash
```

The `geometry` and `simulator` packages have standalone CMake builds independent of colcon — use their own scripts:

```bash
# Geometry (standalone, non-ROS library)
cd src/geometry && ./GeometryBuild.sh
cd src/geometry && ./GeometryTest.sh

# Simulator (standalone)
cd src/simulator && ./SimulatorBuild.sh
cd src/simulator && ./SimulatorRun.sh
```

To build a single ROS package:

```bash
colcon build --symlink-install --packages-select <package_name>
```

## Architecture

### Package Layout

```
src/
  communication/       # ROS-agnostic communication abstraction (library)
  diagnostics/         # Status codes, logging sink (library, header-only types)
  localizer/           # Localizer ROS 2 node (executable)
  manager/             # Manager concept definition (header-only)
  geometry/            # Pure C++ geometry (standalone, no ROS)
  simulator/           # Pure C++ 2D simulator (standalone, no ROS)
  interfaces/
    Controllers/
      localizer_interfaces/   # ROS 2 srv/msg for localizer
      navigator_interfaces/   # ROS 2 srv/msg for navigator
    HAL/
      motor_interfaces/       # ROS 2 msg for motor feedback
```

### Key Design Patterns

**Concept-driven interfaces** — the `communication` package defines all inter-component contracts using C++20 concepts (`Concepts.hpp`). Nothing inherits from a base class; satisfaction of a concept is checked via `static_assert`. `RosCommunicatorApp` has `static_assert(CommunicatorApp<RosCommunicatorApp>)` immediately after its class definition.

**Communication concept hierarchy** (from `Concepts.hpp`):
- `Publisher<T, Msg>` / `Subscriber<T, Msg>` — single-channel primitives
- `MessengerApp<T>` — manages a dynamic set of pub/sub channels; `Login`/`Logout` map to creating/destroying the underlying ROS node
- `CommunicatorApp<T>` — extends `MessengerApp` with service clients and servers
- `StreamingApp<T>` — manages streaming channels (`Streamer`/`Audience`)
- `FullCommunicatorApp<T>` — `CommunicatorApp && StreamingApp`
- `Communicator<T,M,C,FC,S>` — top-level; installs `MessengerApp`, `CommunicatorApp`, etc. as named "apps" and calls `Run()`

**`Communicator` type alias** (`Communicator.hpp`) — currently `std::variant<RosCommunicatorApp>`, enabling compile-time dispatch between transport backends without virtual dispatch.

**STATUS / Result pattern** (`diagnostics/Status.hpp`) — all functions that can fail return `STATUS` (an `int32_t` enum). Richer error propagation uses `Result<T>` = `tl::expected<T, Log>` and `Failure` = `tl::unexpected<Log>`. The `tl::expected` header is a third-party dependency.

**Manager concept** (`src/manager/Manager.hpp`) — defines `IsManager<T>` which requires `Prepare`, `Cleanup`, `Start`, `Stop`, and a `Communicator` member that satisfies `Communication::IsCommunicator`. Each subsystem (Localizer, Navigator, etc.) implements this concept.

**ROS isolation** — business logic (geometry, simulator) is kept entirely free of ROS. ROS dependencies are confined to the `communication`, `localizer`, `diagnostics` packages and the interfaces packages.

### Interface Packages

All ROS 2 message and service types are defined in standalone interface packages under `src/interfaces/` to decouple build dependencies. Packages that need a message type depend only on the matching `*_interfaces` package.

### devcontainer

The `.devcontainer/devcontainer.json` uses `docker/Dockerfile.gazebo` and runs as user `rocket`. The workspace is mounted at `/home/rocket/HereBeRobots/`. Use `--net=host` is set for ROS 2 DDS discovery.
