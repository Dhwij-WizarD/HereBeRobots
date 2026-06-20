#include <communication/Communicator.hpp>
#include <std_msgs/msg/string.hpp>
#include <rclcpp/logging.hpp>
#include <atomic>
#include <chrono>
#include <csignal>
#include <string>
#include <thread>

using namespace HBR::Communication;
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
  app->Login("pub_node");
  app->CreatePublisher<std_msgs::msg::String>("/hbr/test/chatter");

  auto pub = app->GetPublisher<std_msgs::msg::String>("/hbr/test/chatter");

  int count = 0;
  while (sRunning) {
    std_msgs::msg::String msg;
    msg.data = "hello " + std::to_string(count++);
    pub->Publish(msg);
    RCLCPP_INFO(rclcpp::get_logger("pub_app"), "Published: '%s'", msg.data.c_str());
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
  }

  return 0;
}
