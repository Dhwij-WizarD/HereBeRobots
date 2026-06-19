#include <gtest/gtest.h>
#include <communication/Communicator.hpp>
#include <std_msgs/msg/int32.hpp>
#include <chrono>
#include <memory>

using HBR::Communication::Communicator;
using HBR::Communication::RosCommunicatorApp;
using namespace HBR::Diagnostics;
using namespace std::chrono_literals;

// Two apps pre-installed in main():
//   "ros"     — CommunicatorApp, Setup() called (owns rclcpp context)
//   "ros-msg" — MessengerApp
static Communicator * gComm = nullptr;

// ─── Structural ───────────────────────────────────────────────────────────────

TEST(Communicator, GetCommunicatorAppReturnsNonNull)
{
  EXPECT_NE(gComm->GetCommunicatorApp<RosCommunicatorApp>("ros"), nullptr);
}

TEST(Communicator, GetMessengerAppReturnsNonNull)
{
  EXPECT_NE(gComm->GetMessengerApp<RosCommunicatorApp>("ros-msg"), nullptr);
}

TEST(Communicator, GetNonexistentReturnsNullptr)
{
  EXPECT_EQ(gComm->GetCommunicatorApp<RosCommunicatorApp>("nope"), nullptr);
  EXPECT_EQ(gComm->GetMessengerApp<RosCommunicatorApp>("nope"), nullptr);
}

TEST(Communicator, GetReturnsSameInstance)
{
  auto * a = gComm->GetCommunicatorApp<RosCommunicatorApp>("ros");
  auto * b = gComm->GetCommunicatorApp<RosCommunicatorApp>("ros");
  ASSERT_NE(a, nullptr);
  EXPECT_EQ(a, b);
}

// ─── Functional (needs rclcpp live — runs before uninstall suite) ─────────────

class CommunicatorFunctionalTest : public ::testing::Test
{
protected:
  void SetUp() override
  {
    app_ = gComm->GetCommunicatorApp<RosCommunicatorApp>("ros");
    ASSERT_NE(app_, nullptr);
    ASSERT_EQ(app_->Login("communicator_test_node"), OK);
  }
  void TearDown() override {app_->Logout();}
  RosCommunicatorApp * app_ = nullptr;
};

TEST_F(CommunicatorFunctionalTest, InstalledAppAcceptsPubSub)
{
  EXPECT_EQ(app_->CreatePublisher<std_msgs::msg::Int32>("/comm_test/pub"), OK);
  EXPECT_NE(app_->GetPublisher<std_msgs::msg::Int32>("/comm_test/pub"), nullptr);
}

// ─── Uninstall (LAST — destructor fires rclcpp::shutdown()) ──────────────────

TEST(CommunicatorUninstall, UninstallExistingReturnsOk)
{
  ASSERT_EQ(gComm->InstallCommunicatorApp<RosCommunicatorApp>("tmp"), OK);
  EXPECT_EQ(gComm->UninstallApp("tmp"), OK);  // ~RosCommunicatorApp → rclcpp::shutdown()
}

TEST(CommunicatorUninstall, UninstallNonexistentReturnsError)
{
  EXPECT_EQ(gComm->UninstallApp("nope"), ERROR);
}

TEST(CommunicatorUninstall, GetAfterUninstallReturnsNullptr)
{
  EXPECT_EQ(gComm->UninstallApp("ros-msg"), OK);
  EXPECT_EQ(gComm->GetMessengerApp<RosCommunicatorApp>("ros-msg"), nullptr);
}

// ─── Entry point ──────────────────────────────────────────────────────────────

int main(int argc, char ** argv)
{
  Communicator comm;
  gComm = &comm;

  comm.InstallCommunicatorApp<RosCommunicatorApp>("ros");
  comm.InstallMessengerApp<RosCommunicatorApp>("ros-msg");

  comm.GetCommunicatorApp<RosCommunicatorApp>("ros")->Setup(argc, argv);

  ::testing::InitGoogleTest(&argc, argv);
  int result = RUN_ALL_TESTS();

  // comm goes out of scope → remaining apps destroyed → rclcpp::shutdown()
  // (idempotent — already called by CommunicatorUninstall tests above)
  return result;
}
