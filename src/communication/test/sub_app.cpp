#include <communication/RosCommunicatorApp.hpp>
#include <std_msgs/msg/string.hpp>
#include <rclcpp/rclcpp.hpp>
#include <chrono>
#include <thread>

int main(int argc, char ** argv)
{
  using HBR::Diagnostics::STATUS;
  using HBR::Diagnostics::OK;

  HBR::Communication::RosCommunicatorApp app;
  app.Setup(argc, argv);
  app.Login("sub_node");

  auto cb = [](const std_msgs::msg::String & msg) -> STATUS {
      RCLCPP_INFO(rclcpp::get_logger("sub_app"), "Received: '%s'", msg.data.c_str());
      return OK;
    };
  app.CreateSubscriber<std_msgs::msg::String>("/hbr/test/chatter", cb);

  while (rclcpp::ok()) {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }

  app.Logout();
  rclcpp::shutdown();
  return 0;
}
