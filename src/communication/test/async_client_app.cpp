#include <communication/Communicator.hpp>
#include <std_srvs/srv/trigger.hpp>
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <csignal>
#include <iostream>
#include <mutex>

using namespace HBR::Communication;
using namespace HBR::Diagnostics;
using CommunicationBackend = RosCommunicatorApp;

static std::mutex gMtx;
static std::condition_variable gCv;
static std::atomic<bool> gDone{false};

static void notifyDone()
{
  gDone.store(true, std::memory_order_relaxed);
  gCv.notify_one();
}

static void onSignal(int) {notifyDone();}

STATUS ResponseHandler(const std_srvs::srv::Trigger::Response & res)
{
  std::cout << "success: " << res.success << ", message: " << res.message << '\n';
  notifyDone();
  return OK;
}

STATUS TimeoutHandler()
{
  std::cout << "Timeout!\n";
  notifyDone();
  return OK;
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
  app->Login("client");
  app->CreateClient<std_srvs::srv::Trigger>("/trigger");

  auto client = app->GetClient<std_srvs::srv::Trigger>("/trigger");
  if (client == nullptr) {return -1;}

  if (!client->WaitForService(std::chrono::seconds(2))) {
    std::cout << "service unavailable\n";
    return -1;
  }

  std_srvs::srv::Trigger::Request req;
  if (client->AsyncRequest(req, std::chrono::seconds(15), ResponseHandler, TimeoutHandler) != OK) {
    return -1;
  }

  while (!gDone.load()) {
    std::cout << "waiting.....\n";
    std::unique_lock<std::mutex> lock(gMtx);
    gCv.wait_for(lock, std::chrono::milliseconds(500), [] {return gDone.load();});
  }

  return 0;
}
