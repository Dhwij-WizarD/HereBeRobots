#pragma once
#include <communication/Concepts.hpp>
#include <rclcpp/rclcpp.hpp>
#include <future>
#include <memory>
#include <mutex>
#include <optional>
#include <thread>
#include <unordered_map>

namespace HBR::Communication
{

// ---------------------------------------------------------------------------
// RosPublisher<MSG> — adapter satisfying Publisher<RosPublisher<MSG>, MSG>.
// Returned by GetPublisher<MSG>(); nodes call Publish() without touching rclcpp.
// ---------------------------------------------------------------------------
template<typename MSG>
class RosPublisher
{
public:
  explicit RosPublisher(typename rclcpp::Publisher<MSG>::SharedPtr pub)
  : pPub(std::move(pub)) {}

  STATUS Publish(const MSG & msg)
  {
    pPub->publish(msg);
    return OK;
  }

private:
  typename rclcpp::Publisher<MSG>::SharedPtr pPub;
};

// ---------------------------------------------------------------------------
// RosClient<SRV> — adapter satisfying Client<RosClient<SRV>, SRV>.
// WaitForService / Request are the only APIs nodes should call.
// ---------------------------------------------------------------------------
template<typename SRV>
class RosClient
{
public:
  explicit RosClient(typename rclcpp::Client<SRV>::SharedPtr client)
  : pClient(std::move(client)) {}

  bool WaitForService(std::chrono::nanoseconds timeout)
  {
    return pClient->wait_for_service(timeout);
  }

  std::optional<typename SRV::Response> Request(
    const typename SRV::Request & req,
    std::chrono::nanoseconds timeout)
  {
    auto future = pClient->async_send_request(
      std::make_shared<typename SRV::Request>(req));
    if (future.wait_for(timeout) != std::future_status::ready) {return std::nullopt;}
    return *future.get();
  }

private:
  typename rclcpp::Client<SRV>::SharedPtr pClient;
};

// ---------------------------------------------------------------------------
// RosCommunicatorApp
// ---------------------------------------------------------------------------
class RosCommunicatorApp
{
public:
  RosCommunicatorApp() = default;
  ~RosCommunicatorApp();

  STATUS Setup(int argc, char ** argv);

  STATUS Login(const std::string & id);
  STATUS Logout();

  template<typename MSG>
  STATUS CreatePublisher(const std::string & name)
  {
    if (!pNode) {return UNINITIALIZED;}
    publishers[name] = std::make_shared<RosPublisher<MSG>>(
      pNode->create_publisher<MSG>(name, 10));
    return OK;
  }

  template<typename MSG>
  std::shared_ptr<RosPublisher<MSG>> GetPublisher(const std::string & name)
  {
    auto it = publishers.find(name);
    if (!pNode || it == publishers.end()) {return nullptr;}
    return std::static_pointer_cast<RosPublisher<MSG>>(it->second);
  }

  template<typename MSG>
  STATUS CreateSubscriber(const std::string & name, Callback<MSG> cb)
  {
    if (!pNode) {return UNINITIALIZED;}
    subscribers[name] = pNode->create_subscription<MSG>(
      name, 10,
      [cb](const MSG & msg) {cb(msg);});
    return OK;
  }

  STATUS DeletePublisher(const std::string & name);
  STATUS DeleteSubscriber(const std::string & name);

  template<ServiceInterface SRV>
  STATUS CreateClient(const std::string & name)
  {
    if (!pNode) {return UNINITIALIZED;}
    clients[name] = std::make_shared<RosClient<SRV>>(
      pNode->create_client<SRV>(name));
    return OK;
  }

  template<ServiceInterface SRV>
  std::shared_ptr<RosClient<SRV>> GetClient(const std::string & name)
  {
    auto it = clients.find(name);
    if (!pNode || it == clients.end()) {return nullptr;}
    return std::static_pointer_cast<RosClient<SRV>>(it->second);
  }

  template<ServiceInterface SRV>
  STATUS CreateService(
    const std::string & name,
    ResponderCallback<typename SRV::Request, typename SRV::Response> rcb)
  {
    if (!pNode) {return UNINITIALIZED;}
    services[name] = pNode->create_service<SRV>(
      name,
      [rcb](typename SRV::Request::SharedPtr req,
      typename SRV::Response::SharedPtr res)
      {*res = rcb(*req);});
    return OK;
  }

  STATUS DeleteClient(const std::string & name);
  STATUS DeleterService(const std::string & name);

private:
  enum class ThreadState { Idle, Running };

  void Run();

  std::thread t;
  std::mutex mtx;
  ThreadState state {ThreadState::Idle};
  std::promise<void> spinStarted;

  rclcpp::Node::SharedPtr pNode;
  std::unique_ptr<rclcpp::executors::MultiThreadedExecutor> pExecutor;
  rclcpp::CallbackGroup::SharedPtr sharedGroup;
  rclcpp::CallbackGroup::SharedPtr clientGroup;

  // Type-erased storage — publishers hold RosPublisher<MSG>, clients hold RosClient<SRV>.
  std::unordered_map<std::string, std::shared_ptr<void>> publishers;
  std::unordered_map<std::string, rclcpp::SubscriptionBase::SharedPtr> subscribers;
  std::unordered_map<std::string, std::shared_ptr<void>> clients;
  std::unordered_map<std::string, rclcpp::ServiceBase::SharedPtr> services;
};

static_assert(CommunicatorApp<RosCommunicatorApp>);
} // namespace HBR::Communication
