#pragma once

#include <chrono>
#include <concepts>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <std_msgs/msg/empty.hpp>
#include <std_srvs/srv/trigger.hpp>
#include <diagnostics/Status.hpp>

namespace HBR::Communication
{

using namespace Diagnostics;

template<typename T>
using Callback = std::function<STATUS(const T &)>;

template<typename RequestT, typename ResponseT>
using ResponderCallback = std::function<ResponseT(const RequestT &)>;

// ─── Call-site concepts ────────────────────────────────────────────────────────
// Each adapter type carries its associated data/service type as a nested alias
// (MessageType / ServiceType), making these concepts self-contained.
// Applied by node code when the type is already known, and by static_asserts
// in backend headers to verify concrete adapters satisfy the contract.

// All communicatable data in this system is a ROS2 interface type.
// Create interface types under src/interfaces/ and link against them.
// No compile-time Communicatable concept — rclcpp enforces this at the call site.

template<typename T>
concept Publisher =
  requires(T pub)
    {
    typename T::MessageType;
  {pub.Publish(std::declval<const typename T::MessageType &>())}->std::same_as<STATUS>;
};

template<typename T>
concept Subscriber =
  requires(T subscriber, Callback<typename T::MessageType> callback)
    {
    typename T::MessageType;
        &T::Setup;
  {subscriber.StartHandler(callback)}->std::same_as<STATUS>;
};

template<typename T>
concept ServiceInterface =
  requires
{
  typename T::Request;
  typename T::Response;
};

template<typename T>
concept Service =
  requires(T service, const typename T::ServiceType::Request & req)
    {
    typename T::ServiceType;
    requires ServiceInterface<typename T::ServiceType>;
  {service.Respond(req)}->std::same_as<typename T::ServiceType::Response>;
};

template<typename T>
concept Client =
  requires(T client,
    const typename T::ServiceType::Request & req,
    std::chrono::nanoseconds timeout,
    std::function<STATUS(const typename T::ServiceType::Response &)> rcb,
    std::function<STATUS()> tfn)
    {
    typename T::ServiceType;
    requires ServiceInterface<typename T::ServiceType>;
  {client.WaitForService(timeout)}->std::same_as<bool>;
  {client.Request(req, timeout)}->std::same_as<std::optional<typename T::ServiceType::Response>>;
  {client.AsyncRequest(req, timeout, rcb, tfn)}->std::same_as<STATUS>;
};

template<typename T, typename StreamT>
concept Streamer =
  requires(T streamer, const StreamT & data)
    {
  {streamer.StartStream()}->std::same_as<STATUS>;
  {streamer.StopStream()}->std::same_as<STATUS>;
  {streamer.Send(data)}->std::same_as<STATUS>;
};

template<typename T, typename StreamT>
concept Audience =
  requires(T audience, Callback<StreamT> callback)
    {
  {audience.JoinStream(callback)}->std::same_as<STATUS>;
  {audience.LeaveStream()}->std::same_as<STATUS>;
};

// ─── App-management concepts ───────────────────────────────────────────────────
// Describe the management API shape of a communication backend.
// std_msgs::msg::Empty and std_srvs::srv::Trigger are the architectural probes —
// every communicatable type in this system is a ROS2 interface type, so these
// are accurate representatives, not arbitrary choices.

template<typename T>
concept MessengerApp =
  requires(T messenger, const std::string & name, Callback<std_msgs::msg::Empty> cb)
    {
  {messenger.Login(name)}->std::same_as<STATUS>;
  {messenger.Logout()}->std::same_as<STATUS>;
  {messenger.template CreatePublisher<std_msgs::msg::Empty>(name)}->std::same_as<STATUS>;
  {messenger.template CreateSubscriber<std_msgs::msg::Empty>(name, cb)}->std::same_as<STATUS>;
  {messenger.DeletePublisher(name)}->std::same_as<STATUS>;
  {messenger.DeleteSubscriber(name)}->std::same_as<STATUS>;
    requires Publisher<
    typename decltype(messenger.template GetPublisher<std_msgs::msg::Empty>(name))::element_type>;
};

template<typename T>
concept CommunicatorApp =
  MessengerApp<T>&&
  requires(T comm,
    const std::string & name,
    ResponderCallback<std_srvs::srv::Trigger::Request, std_srvs::srv::Trigger::Response> rcb)
    {
    &T::Setup;
  {comm.template CreateClient<std_srvs::srv::Trigger>(name)}->std::same_as<STATUS>;
  {comm.template CreateService<std_srvs::srv::Trigger>(name, rcb)}->std::same_as<STATUS>;
    requires Client<
    typename decltype(comm.template GetClient<std_srvs::srv::Trigger>(name))::element_type>;
  {comm.DeleteClient(name)}->std::same_as<STATUS>;
  {comm.DeleterService(name)}->std::same_as<STATUS>;
};

template<typename T>
concept StreamerApp =
  requires(T app, const std::string & name)
    {
  {app.Login(name)}->std::same_as<STATUS>;
  {app.Logout()}->std::same_as<STATUS>;
  {app.template CreateStream<std_msgs::msg::Empty>(name)}->std::same_as<STATUS>;
  {app.DeleteStream(name)}->std::same_as<STATUS>;
  {app.template JoinStream<std_msgs::msg::Empty>(name)}->std::same_as<STATUS>;
  {app.LeaveStream(name)}->std::same_as<STATUS>;
};

template<typename T>
concept FullCommunicatorApp = CommunicatorApp<T>&& StreamerApp<T>;

// ─── Proxy stubs ───────────────────────────────────────────────────────────────
// Minimal types used to static_assert the concepts above in isolation.
// ROS2 probe types appear here intentionally — see the architectural note above.

struct ProxyPublisher
{
  using MessageType = std_msgs::msg::Empty;
  STATUS Publish(const MessageType &) {return OK;}
};

static_assert(Publisher<ProxyPublisher>);

struct ProxyClient
{
  using ServiceType = std_srvs::srv::Trigger;
  bool WaitForService(std::chrono::nanoseconds) {return false;}
  std::optional<ServiceType::Response> Request(
    const ServiceType::Request &, std::chrono::nanoseconds)
  {return std::nullopt;}
  STATUS AsyncRequest(
    const ServiceType::Request &,
    std::chrono::nanoseconds,
    std::function<STATUS(const ServiceType::Response &)>,
    std::function<STATUS()> = [] {return OK;})
  {return OK;}
};

static_assert(Client<ProxyClient>);

struct ProxyStreamerApp
{
  STATUS Login(const std::string & name);
  STATUS Logout();
  template<typename T>
  STATUS CreateStream(const std::string & name);
  STATUS DeleteStream(const std::string & name);
  template<typename T>
  STATUS JoinStream(const std::string & name);
  STATUS LeaveStream(const std::string & name);
};

static_assert(StreamerApp<ProxyStreamerApp>);

struct ProxyFullCommunicatorApp : ProxyStreamerApp
{
  STATUS Setup(int, char **);
  template<typename T>
  STATUS CreatePublisher(const std::string &);
  template<typename T>
  std::shared_ptr<ProxyPublisher> GetPublisher(const std::string &);
  template<typename T>
  STATUS CreateSubscriber(const std::string &, Callback<T>);
  STATUS DeletePublisher(const std::string &);
  STATUS DeleteSubscriber(const std::string &);
  template<typename SRV>
  STATUS CreateClient(const std::string &);
  template<typename SRV>
  STATUS CreateService(
    const std::string &,
    ResponderCallback<typename SRV::Request, typename SRV::Response>);
  STATUS DeleteClient(const std::string &);
  STATUS DeleterService(const std::string &);
  template<typename SRV>
  std::shared_ptr<ProxyClient> GetClient(const std::string &);
};

static_assert(MessengerApp<ProxyFullCommunicatorApp>);
static_assert(CommunicatorApp<ProxyFullCommunicatorApp>);
static_assert(FullCommunicatorApp<ProxyFullCommunicatorApp>);

} // namespace HBR::Communication
