#include <communication/RosCommunicatorApp.hpp>
#include <rclcpp/rclcpp.hpp>

namespace HBR::Communication
{
using namespace Diagnostics;

STATUS RosCommunicatorApp::Setup(int argc, char ** argv)
{
  rclcpp::init(argc, argv);
  return OK;
}

RosCommunicatorApp::~RosCommunicatorApp()
{
  Logout();
  rclcpp::shutdown();
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
