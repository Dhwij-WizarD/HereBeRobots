#include <communication/Communicator.hpp>
#include <std_srvs/srv/trigger.hpp>
#include <chrono>
#include <iostream>

using namespace HBR::Communication;
using namespace HBR::Diagnostics;
using CommunicationBackend = RosCommunicatorApp;

int main(int argc, char ** argv)
{
  Communicator comm;

  if (comm.InstallCommunicatorApp<CommunicationBackend>("ros") != OK) {return -1;}

  auto * app = comm.GetCommunicatorApp<CommunicationBackend>("ros");
  if (app == nullptr) {return -1;}

  app->Setup(argc, argv);
  app->Login("client");
  app->CreateClient<std_srvs::srv::Trigger>("/trigger");

  auto client = app->GetClient<std_srvs::srv::Trigger>("/trigger");
  if (client == nullptr) {return -1;}

  if (!client->WaitForService(std::chrono::seconds(2))) {
    std::cout << "service unavailable\n";
    return -1;
  }

  std_srvs::srv::Trigger::Request req;
  auto result = client->Request(req, std::chrono::seconds(2));

  if (!result) {
    std::cout << "no response within timeout\n";
    return -1;
  }

  std::cout << "success: " << result->success << ", msg: " << result->message << '\n';
  return 0;
}
