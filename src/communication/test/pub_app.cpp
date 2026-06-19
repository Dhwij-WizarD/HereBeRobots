#include <communication/Communicator.hpp>
#include <std_msgs/msg/string.hpp>
#include <rclcpp/rclcpp.hpp>
#include <chrono>
#include <thread>

int main(int argc, char ** argv)
{
  using namespace HBR::Communication;

  Communicator comm;
  comm.InstallCommunicatorApp<RosCommunicatorApp>("ros");

  auto * app = comm.GetCommunicatorApp<RosCommunicatorApp>("ros");
  app->Setup(argc, argv);
  app->Login("pub_node");
  app->CreatePublisher<std_msgs::msg::String>("/hbr/test/chatter");

  auto pub = app->GetPublisher<std_msgs::msg::String>("/hbr/test/chatter");

  int count = 0;
  while (rclcpp::ok()) {
    std_msgs::msg::String msg;
    msg.data = "hello " + std::to_string(count++);
    pub->publish(msg);
    RCLCPP_INFO(rclcpp::get_logger("pub_app"), "Published: '%s'", msg.data.c_str());
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
  }

  // app->Logout(); Logs out on destruction.
  return 0;
}
