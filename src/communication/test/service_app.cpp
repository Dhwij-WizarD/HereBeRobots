#include <communication/Communicator.hpp>
#include <std_srvs/srv/trigger.hpp>
#include <chrono>
#include <diagnostics/Status.hpp>


std_srvs::srv::Trigger::Response Service(const std_srvs::srv::Trigger::Request & req) {
    (void) req;
    std_srvs::srv::Trigger::Response res;
    res.success = true;
    res.message = "Whatzupp!";
    return res;
}

int main(int argc, char ** argv)
{
  using namespace HBR::Communication;

  Communicator comm;

  if (comm.InstallCommunicatorApp<RosCommunicatorApp>("ros") != OK) {
    return -1;
  }

  RosCommunicatorApp * app = comm.GetCommunicatorApp<RosCommunicatorApp>("ros");

  if (app == nullptr) {
    return -1;
  }

  app->Setup(argc, argv);
  app->Login("service");

  app->CreateService<std_srvs::srv::Trigger>("/trigger", &Service);

  while (rclcpp::ok()) {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }

  return 0;
}
