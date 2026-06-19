#pragma once
#include <communication/Concepts.hpp>
#include <rclcpp/rclcpp.hpp>
#include <future>
#include <thread>
#include <mutex>
#include <unordered_map>

namespace HBR::Communication
{
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
    publishers[name] = pNode->create_publisher<MSG>(name, 10);
    return OK;
  }

  template<typename MSG>
  rclcpp::Publisher<MSG>::SharedPtr GetPublisher(const std::string & name)
  {
    auto it = publishers.find(name);
    if (!pNode || it == publishers.end()) {return nullptr;}
    return std::static_pointer_cast<rclcpp::Publisher<MSG>>(it->second);
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

  template<typename SRV>
  STATUS CreateClient(const std::string & name)
  {
    if (!pNode) {return UNINITIALIZED;}
    clients[name] = pNode->create_client<SRV>(name);
    return OK;
  }

  template<typename SRV>
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

  std::unordered_map<std::string, rclcpp::PublisherBase::SharedPtr> publishers;
  std::unordered_map<std::string, rclcpp::SubscriptionBase::SharedPtr> subscribers;
  std::unordered_map<std::string, rclcpp::ClientBase::SharedPtr> clients;
  std::unordered_map<std::string, rclcpp::ServiceBase::SharedPtr> services;
};

static_assert(CommunicatorApp<RosCommunicatorApp>);
} // namespace HBR::Communication
