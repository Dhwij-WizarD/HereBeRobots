#include <gtest/gtest.h>
#include <communication/Communicator.hpp>
#include <std_msgs/msg/int32.hpp>
#include <std_srvs/srv/set_bool.hpp>
#include <chrono>
#include <future>
#include <memory>
#include <thread>

using HBR::Communication::Communicator;
using HBR::Communication::RosCommunicatorApp;
using namespace HBR::Diagnostics;
using namespace std::chrono_literals;

// One app per process — owns the rclcpp context via Setup().
// All tests share it; Login/Logout provide per-test node isolation.
static RosCommunicatorApp * gApp = nullptr;

// ─── Fixture ──────────────────────────────────────────────────────────────────

class RosCommunicatorAppTest : public ::testing::Test
{
protected:
  void SetUp() override   {ASSERT_EQ(gApp->Login("test_node"), OK);}
  void TearDown() override {gApp->Logout();}
};

// ─── Lifecycle ────────────────────────────────────────────────────────────────

TEST(Lifecycle, LoginLogout)
{
  EXPECT_EQ(gApp->Login("lifecycle_node"), OK);
  EXPECT_EQ(gApp->Logout(), OK);
  EXPECT_EQ(gApp->Logout(), OK);   // idempotent
}

TEST(Lifecycle, ReLogin)
{
  EXPECT_EQ(gApp->Login("relogin_node"), OK);
  EXPECT_EQ(gApp->Logout(), OK);
  EXPECT_EQ(gApp->Login("relogin_node"), OK);
  EXPECT_EQ(gApp->Logout(), OK);
}

TEST(Lifecycle, CreateBeforeLoginReturnsUninitialized)
{
  // Previous lifecycle tests always Logout() — gApp is idle here.
  auto cb = [](const std_msgs::msg::Int32 &) -> STATUS {return OK;};
  auto rcb = [](const std_srvs::srv::SetBool::Request &)
    {return std_srvs::srv::SetBool::Response{};};

  EXPECT_EQ(gApp->CreatePublisher<std_msgs::msg::Int32>("t"), UNINITIALIZED);
  EXPECT_EQ(gApp->CreateSubscriber<std_msgs::msg::Int32>("t", cb), UNINITIALIZED);
  EXPECT_EQ(gApp->CreateClient<std_srvs::srv::SetBool>("s"), UNINITIALIZED);
  EXPECT_EQ(gApp->CreateService<std_srvs::srv::SetBool>("s", rcb), UNINITIALIZED);
}

// ─── Publisher ────────────────────────────────────────────────────────────────

TEST_F(RosCommunicatorAppTest, CreateDeletePublisher)
{
  EXPECT_EQ(gApp->CreatePublisher<std_msgs::msg::Int32>("/pub_test/topic"), OK);
  EXPECT_EQ(gApp->DeletePublisher("/pub_test/topic"), OK);
  EXPECT_EQ(gApp->DeletePublisher("/pub_test/topic"), ERROR);
}

TEST_F(RosCommunicatorAppTest, DeleteMissingPublisherReturnsError)
{
  EXPECT_EQ(gApp->DeletePublisher("nonexistent"), ERROR);
}

// ─── Subscriber ───────────────────────────────────────────────────────────────

TEST_F(RosCommunicatorAppTest, CreateDeleteSubscriber)
{
  auto cb = [](const std_msgs::msg::Int32 &) -> STATUS {return OK;};
  EXPECT_EQ(gApp->CreateSubscriber<std_msgs::msg::Int32>("/sub_test/topic", cb), OK);
  EXPECT_EQ(gApp->DeleteSubscriber("/sub_test/topic"), OK);
  EXPECT_EQ(gApp->DeleteSubscriber("/sub_test/topic"), ERROR);
}

TEST_F(RosCommunicatorAppTest, DeleteMissingSubscriberReturnsError)
{
  EXPECT_EQ(gApp->DeleteSubscriber("nonexistent"), ERROR);
}

// ─── Pub/Sub roundtrip ────────────────────────────────────────────────────────

TEST_F(RosCommunicatorAppTest, SubscriberReceivesMessage)
{
  constexpr int32_t kValue = 42;
  auto promise = std::make_shared<std::promise<int32_t>>();
  auto future = promise->get_future();

  auto cb = [promise](const std_msgs::msg::Int32 & msg) -> STATUS {
      try {
        promise->set_value(msg.data);
      } catch (...) {
      }
      return OK;
    };

  ASSERT_EQ(gApp->CreateSubscriber<std_msgs::msg::Int32>("/pubsub_test/sub", cb), OK);

  auto helper = std::make_shared<rclcpp::Node>("helper_pub");
  auto pub = helper->create_publisher<std_msgs::msg::Int32>("/pubsub_test/sub", 10);

  std::this_thread::sleep_for(200ms);

  std_msgs::msg::Int32 msg;
  msg.data = kValue;
  pub->publish(msg);

  ASSERT_EQ(future.wait_for(2s), std::future_status::ready);
  EXPECT_EQ(future.get(), kValue);
}

TEST_F(RosCommunicatorAppTest, PublisherSendsMessage)
{
  constexpr int32_t kValue = 99;
  auto promise = std::make_shared<std::promise<int32_t>>();
  auto future = promise->get_future();

  auto helper = std::make_shared<rclcpp::Node>("helper_sub");
  auto sub = helper->create_subscription<std_msgs::msg::Int32>(
    "/pubsub_test/pub", 10,
    [promise](std_msgs::msg::Int32::ConstSharedPtr msg) {
      try {
        promise->set_value(msg->data);
      } catch (...) {
      }
    });

  rclcpp::executors::SingleThreadedExecutor exec;
  exec.add_node(helper);
  std::thread helper_thread([&exec] {exec.spin();});

  ASSERT_EQ(gApp->CreatePublisher<std_msgs::msg::Int32>("/pubsub_test/pub"), OK);

  std::this_thread::sleep_for(200ms);

  std_msgs::msg::Int32 msg;
  msg.data = kValue;
  auto pub = gApp->GetPublisher<std_msgs::msg::Int32>("/pubsub_test/pub");
  ASSERT_NE(pub, nullptr);
  pub->publish(msg);

  auto status = future.wait_for(2s);

  exec.cancel();
  helper_thread.join();

  ASSERT_EQ(status, std::future_status::ready);
  EXPECT_EQ(future.get(), kValue);
}

// ─── Service ──────────────────────────────────────────────────────────────────

TEST_F(RosCommunicatorAppTest, CreateDeleteService)
{
  auto rcb = [](const std_srvs::srv::SetBool::Request &)
    {return std_srvs::srv::SetBool::Response{};};
  EXPECT_EQ(gApp->CreateService<std_srvs::srv::SetBool>("/svc_test/create", rcb), OK);
  EXPECT_EQ(gApp->DeleterService("/svc_test/create"), OK);
  EXPECT_EQ(gApp->DeleterService("/svc_test/create"), ERROR);
}

TEST_F(RosCommunicatorAppTest, DeleteMissingServiceReturnsError)
{
  EXPECT_EQ(gApp->DeleterService("nonexistent"), ERROR);
}

// ─── Client ───────────────────────────────────────────────────────────────────

TEST_F(RosCommunicatorAppTest, CreateDeleteClient)
{
  EXPECT_EQ(gApp->CreateClient<std_srvs::srv::SetBool>("/client_test/svc"), OK);
  EXPECT_EQ(gApp->DeleteClient("/client_test/svc"), OK);
  EXPECT_EQ(gApp->DeleteClient("/client_test/svc"), ERROR);
}

TEST_F(RosCommunicatorAppTest, DeleteMissingClientReturnsError)
{
  EXPECT_EQ(gApp->DeleteClient("nonexistent"), ERROR);
}

// ─── Service roundtrip ────────────────────────────────────────────────────────

TEST_F(RosCommunicatorAppTest, ServiceRoundTrip)
{
  auto rcb = [](const std_srvs::srv::SetBool::Request & req) {
      std_srvs::srv::SetBool::Response res;
      res.success = req.data;
      res.message = req.data ? "on" : "off";
      return res;
    };
  ASSERT_EQ(gApp->CreateService<std_srvs::srv::SetBool>("/svc_test/roundtrip", rcb), OK);

  auto helper = std::make_shared<rclcpp::Node>("helper_client");
  auto client = helper->create_client<std_srvs::srv::SetBool>("/svc_test/roundtrip");

  rclcpp::executors::SingleThreadedExecutor exec;
  exec.add_node(helper);
  std::thread helper_thread([&exec] {exec.spin();});

  auto cleanup = [&] {
      exec.cancel();
      if (helper_thread.joinable()) {helper_thread.join();}
    };

  if (!client->wait_for_service(2s)) {
    cleanup();
    FAIL() << "Service not available within timeout";
    return;
  }

  auto request = std::make_shared<std_srvs::srv::SetBool::Request>();
  request->data = true;
  auto future = client->async_send_request(request);
  auto fut_status = future.wait_for(2s);

  cleanup();

  ASSERT_EQ(fut_status, std::future_status::ready);
  auto response = future.get();
  EXPECT_TRUE(response->success);
  EXPECT_EQ(response->message, std::string("on"));
}

// ─── Entry point ──────────────────────────────────────────────────────────────

int main(int argc, char ** argv)
{
  Communicator comm;
  comm.InstallCommunicatorApp<RosCommunicatorApp>("ros");
  gApp = comm.GetCommunicatorApp<RosCommunicatorApp>("ros");
  gApp->Setup(argc, argv);   // rclcpp::init() — one context for the whole binary

  ::testing::InitGoogleTest(&argc, argv);
  int result = RUN_ALL_TESTS();

  // comm goes out of scope → ~Communicator → ~RosCommunicatorApp
  //   → Logout() + rclcpp::shutdown()
  return result;
}
