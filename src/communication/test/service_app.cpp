#include <communication/Communicator.hpp>
#include <std_srvs/srv/trigger.hpp>
#include <diagnostics/Status.hpp>
#include <atomic>
#include <chrono>
#include <csignal>
#include <thread>

using namespace HBR::Communication;
using namespace HBR::Diagnostics;
using CommunicationBackend = RosCommunicatorApp;

static std::atomic<bool> sRunning{true};
static void onSignal(int) {sRunning = false;}

static std_srvs::srv::Trigger::Response onTrigger(const std_srvs::srv::Trigger::Request &)
{
  std_srvs::srv::Trigger::Response res;
  res.success = true;
  res.message = "Whatzupp!";
  return res;
}

int main(int argc, char ** argv)
{
  std::signal(SIGINT, onSignal);
  std::signal(SIGTERM, onSignal);

  Communicator comm;

  if (comm.InstallCommunicatorApp<CommunicationBackend>("ros") != OK) {return -1;}

  auto * app = comm.GetCommunicatorApp<CommunicationBackend>("ros");
  if (app == nullptr) {return -1;}

  app->Setup(argc, argv);
  app->Login("service");
  app->CreateService<std_srvs::srv::Trigger>("/trigger", onTrigger);

  while (sRunning) {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }

  return 0;
}
