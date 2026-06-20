#include <communication/Communicator.hpp>
#include <std_srvs/srv/trigger.hpp>
#include <chrono>


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
  app->Login("client");

  app->CreateClient<std_srvs::srv::Trigger>("/trigger");

  rclcpp::Client<std_srvs::srv::Trigger>::SharedPtr client = app->GetClient<std_srvs::srv::Trigger>("/trigger");

  if (!client->wait_for_service(std::chrono::seconds(2))) {
  std::cout << "service unavailable\n";
    return -1;
  }

  std_srvs::srv::Trigger::Request::SharedPtr req = std::make_shared<std_srvs::srv::Trigger::Request>();
  auto future = client->async_send_request(req);
  auto fut_status = future.wait_for(std::chrono::seconds(2));

  if (fut_status == std::future_status::ready) {
    auto response = future.get();
    std::cout << "success: " << response->success << ", msg: " << response->message << '\n';
    return 0;
  }
  std::cout << "didn't receive response in time\n";
  return -1;
}
