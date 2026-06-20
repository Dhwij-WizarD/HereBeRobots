#include <communication/Communicator.hpp>
#include <std_msgs/msg/string.hpp>
#include <rclcpp/logging.hpp>
#include <atomic>
#include <chrono>
#include <csignal>
#include <thread>

using namespace HBR::Communication;
using namespace HBR::Diagnostics;
using CommunicationBackend = RosCommunicatorApp;

static std::atomic<bool> sRunning{true};
static void onSignal(int) {sRunning = false;}

int main(int argc, char ** argv)
{
  std::signal(SIGINT, onSignal);
  std::signal(SIGTERM, onSignal);

  Communicator comm;
  comm.InstallCommunicatorApp<CommunicationBackend>("ros");

  auto * app = comm.GetCommunicatorApp<CommunicationBackend>("ros");
  app->Setup(argc, argv);
  app->Login("sub_node");

  auto cb = [](const std_msgs::msg::String & msg) -> STATUS {
      RCLCPP_INFO(rclcpp::get_logger("sub_app"), "Received: '%s'", msg.data.c_str());
      return OK;
    };
  app->CreateSubscriber<std_msgs::msg::String>("/hbr/test/chatter", cb);

  while (sRunning) {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }

  return 0;
}
