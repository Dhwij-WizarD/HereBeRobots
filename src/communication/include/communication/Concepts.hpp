#pragma once

#include <chrono>
#include <concepts>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <variant>
#include <diagnostics/Status.hpp>
#include <std_srvs/srv/trigger.hpp>

namespace HBR::Communication
{

using namespace Diagnostics;

template<typename T, typename MessageT>
concept Publisher =
  requires(T publisher, const MessageT & message)
    {
  {publisher.Publish(message)}->std::same_as<STATUS>;
};

template<typename T>
using Callback = std::function<STATUS(const T &)>;

template<typename RequestT, typename ResponseT>
using ResponderCallback = std::function<ResponseT(const RequestT &)>;

template<typename T, typename MessageT>
concept Subscriber =
  requires(T subscriber, Callback<MessageT> callback)
    {
        &T::Setup;
  {subscriber.StartHandler(callback)}->std::same_as<STATUS>;
};

    // Manages a dynamic collection of publishers and subscriptions.
    // Not parameterized by message type — channels of any type can be added.
    // Proxy type int is used in requires to verify the template method shape.
template<typename T>
concept MessengerApp =
  requires(T messenger, const std::string & name, Callback<int> cb)
    {
  {messenger.Login(name)}->std::same_as<STATUS>;
  {messenger.Logout()}->std::same_as<STATUS>;
  {messenger.template CreatePublisher<int>(name)}->std::same_as<STATUS>;
  {messenger.template CreateSubscriber<int>(name, cb)}->std::same_as<STATUS>;
  {messenger.DeletePublisher(name)}->std::same_as<STATUS>;
  {messenger.DeleteSubscriber(name)}->std::same_as<STATUS>;
};

template<typename T, typename RequestT, typename ResponseT>
concept Requester =
  requires(T requester, const RequestT & request)
    {
  {requester.Request(request)}->std::same_as<ResponseT>;
};

template<typename T, typename RequestT, typename ResponseT>
concept Responder =
  requires(T responder, const RequestT & request)
    {
  {responder.Respond(request)}->std::same_as<ResponseT>;
};

template<typename T>
concept ServiceInterface =
  requires(T interface)
    {
        typename T::Request;
        typename T::Response;
};

    // Proxy service type for single-template-param concept checks.
static_assert(ServiceInterface<std_srvs::srv::Trigger>);

    // Satisfied by any client handle returned by GetClient<SRV>().
    // WaitForService blocks until the server is discoverable or timeout expires.
    // Request sends one call synchronously and returns the response, or nullopt on timeout.
template<typename T, typename SRV>
concept Client =
  ServiceInterface<SRV>&&
  requires(T client, const typename SRV::Request & req, std::chrono::nanoseconds timeout)
    {
  {client.WaitForService(timeout)}->std::same_as<bool>;
  {client.Request(req, timeout)}->std::same_as<std::optional<typename SRV::Response>>;
};

template<typename T>
concept HasCreateClientDualParam =
  requires(T comm, const std::string & name)
    {
  {comm.template CreateClient<int, int>(name)}->std::same_as<STATUS>;
};

template<typename T>
concept HasCreateClientSingleParam =
  requires(T comm, const std::string & name)
    {
  {comm.template CreateClient<std_srvs::srv::Trigger>(name)}->std::same_as<STATUS>;
};

template<typename T>
concept HasCreateServiceDualParam =
  requires(T comm, const std::string & name, ResponderCallback<int, int> rcb)
    {
  {comm.template CreateService<int, int>(name, rcb)}->std::same_as<STATUS>;
};

template<typename T>
concept HasCreateServiceSingleParam =
  requires(T comm, const std::string & name,
    ResponderCallback<std_srvs::srv::Trigger::Request, std_srvs::srv::Trigger::Response> rcb)
    {
  {comm.template CreateService<std_srvs::srv::Trigger>(name, rcb)}->std::same_as<STATUS>;
};

template<typename T>
concept CommunicatorApp =
  MessengerApp<T>&&
  (HasCreateClientDualParam<T>|| HasCreateClientSingleParam<T>) &&
  (HasCreateServiceDualParam<T>|| HasCreateServiceSingleParam<T>) &&
  requires(T comm, const std::string & name)
    {
  &T::Setup;
  comm.template GetClient<std_srvs::srv::Trigger>(name);
  {comm.DeleteClient(name)}->std::same_as<STATUS>;
  {comm.DeleterService(name)}->std::same_as<STATUS>;
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

template<typename T>
concept StreamerApp =
  requires(T app, const std::string & name)
    {
  {app.Login(name)}->std::same_as<STATUS>;
  {app.Logout()}->std::same_as<STATUS>;
  {app.template CreateStream<int>(name)}->std::same_as<STATUS>;
  {app.DeleteStream(name)}->std::same_as<STATUS>;
  {app.template JoinStream<int>(name)}->std::same_as<STATUS>;
  {app.LeaveStream(name)}->std::same_as<STATUS>;
};

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

    // Proxy type that satisfies FullCommunicatorApp (CommunicatorApp + StreamerApp).
struct ProxyFullCommunicatorApp : ProxyStreamerApp
{
  STATUS Setup(int, char **);
  template<typename T>
  STATUS CreatePublisher(const std::string &);
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
  void * GetClient(const std::string &);
};

template<typename T>
concept FullCommunicatorApp =
  CommunicatorApp<T>&& StreamerApp<T>;

static_assert(FullCommunicatorApp<ProxyFullCommunicatorApp>);

template<typename T, typename M, typename C, typename FC, typename S>
concept CommunicatorInterface =
  MessengerApp<M>&& CommunicatorApp<C>&& FullCommunicatorApp<FC>&& StreamerApp<S>&&
  requires(
        T communicator,
        const std::string & name
  )
    {
  {communicator.template InstallMessengerApp<M>(name)}->std::same_as<STATUS>;
  {communicator.template InstallCommunicatorApp<C>(name)}->std::same_as<STATUS>;
  {communicator.template InstallFullCommunicatorApp<FC>(name)}->std::same_as<STATUS>;
  {communicator.template InstallStreamingApp<S>(name)}->std::same_as<STATUS>;
  {communicator.UninstallApp(name)}->std::same_as<STATUS>;
};
}
