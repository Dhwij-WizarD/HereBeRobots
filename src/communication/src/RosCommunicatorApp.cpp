#include <communication/RosCommunicatorApp.hpp>
#include <rclcpp/rclcpp.hpp>
#include <mutex>

namespace
{
std::mutex gRclcppMtx;
int gRclcppRefCount = 0;
}

namespace HBR::Communication
{
using namespace Diagnostics;

void RosCommunicatorApp::RclcppHandle::acquire(int argc, char ** argv)
{
  std::lock_guard<std::mutex> lock(gRclcppMtx);
  if (gRclcppRefCount++ == 0) {
    rclcpp::init(argc, argv);
  }
}

void RosCommunicatorApp::RclcppHandle::release()
{
  std::lock_guard<std::mutex> lock(gRclcppMtx);
  if (--gRclcppRefCount == 0) {
    rclcpp::shutdown();
  }
}

STATUS RosCommunicatorApp::Setup(int argc, char ** argv)
{
  if (!setupCalled) {
    RclcppHandle::acquire(argc, argv);
    setupCalled = true;
  }
  return OK;
}

RosCommunicatorApp::~RosCommunicatorApp()
{
  Logout();
  if (setupCalled) {
    RclcppHandle::release();
  }
}

STATUS RosCommunicatorApp::Login(const std::string & id)
{
  Logout();

  pNode = std::make_shared<rclcpp::Node>(id);
  if (!pNode) {return ERROR;}

  pExecutor = std::make_unique<rclcpp::executors::MultiThreadedExecutor>();
  pExecutor->add_node(pNode);
  spinStarted = {};
  auto started = spinStarted.get_future();
  {
    std::lock_guard lock(mtx);
    state = ThreadState::Running;
  }
  t = std::thread{&RosCommunicatorApp::Run, this};
  started.wait();  // block until Run() has entered spin()
  return OK;
}

STATUS RosCommunicatorApp::Logout()
{
  {
    std::lock_guard lock(mtx);
    if (state == ThreadState::Idle) {return OK;}
    state = ThreadState::Idle;
  }
  pExecutor->cancel();
  if (t.joinable()) {t.join();}

  publishers.clear();
  subscribers.clear();
  clients.clear();
  services.clear();

  pExecutor->remove_node(pNode);
  pExecutor.reset();
  pNode.reset();
  return OK;
}

void RosCommunicatorApp::Run()
{
  spinStarted.set_value();  // unblock Login() — spin() is guaranteed to follow
  pExecutor->spin();
}

STATUS RosCommunicatorApp::DeletePublisher(const std::string & name)
{
  return publishers.erase(name) ? OK : ERROR;
}

STATUS RosCommunicatorApp::DeleteSubscriber(const std::string & name)
{
  return subscribers.erase(name) ? OK : ERROR;
}

STATUS RosCommunicatorApp::DeleteClient(const std::string & name)
{
  return clients.erase(name) ? OK : ERROR;
}

STATUS RosCommunicatorApp::DeleterService(const std::string & name)
{
  return services.erase(name) ? OK : ERROR;
}
}
