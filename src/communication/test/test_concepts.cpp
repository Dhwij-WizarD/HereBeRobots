#include <gtest/gtest.h>
#include <communication/Concepts.hpp>

using namespace HBR::Communication;
using namespace HBR::Diagnostics;

// ─── Bad Publisher types ───────────────────────────────────────────────────────

struct PubNoMessageType
{
  STATUS Publish(const std_msgs::msg::Empty &) {return OK;}
};

struct PubNoPublish
{
  using MessageType = std_msgs::msg::Empty;
};

struct PubWrongReturn
{
  using MessageType = std_msgs::msg::Empty;
  void Publish(const std_msgs::msg::Empty &) {}
};

// ─── Bad Client types ──────────────────────────────────────────────────────────

struct ClientNoServiceType
{
  bool WaitForService(std::chrono::nanoseconds) {return false;}
  std::optional<std_srvs::srv::Trigger::Response> Request(
    const std_srvs::srv::Trigger::Request &, std::chrono::nanoseconds) {return std::nullopt;}
};

struct ClientWrongWaitReturn
{
  using ServiceType = std_srvs::srv::Trigger;
  STATUS WaitForService(std::chrono::nanoseconds) {return OK;}  // should be bool
  std::optional<ServiceType::Response> Request(
    const ServiceType::Request &, std::chrono::nanoseconds) {return std::nullopt;}
  STATUS AsyncRequest(
    const ServiceType::Request &, std::chrono::nanoseconds,
    std::function<STATUS(const ServiceType::Response &)>,
    std::function<STATUS()> = [] {return OK;}) {return OK;}
};

struct ClientMissingAsync
{
  using ServiceType = std_srvs::srv::Trigger;
  bool WaitForService(std::chrono::nanoseconds) {return false;}
  std::optional<ServiceType::Response> Request(
    const ServiceType::Request &, std::chrono::nanoseconds) {return std::nullopt;}
  // No AsyncRequest
};

// ─── Bad MessengerApp / CommunicatorApp types ──────────────────────────────────

struct PartialMessenger
{
  STATUS Login(const std::string &) {return OK;}
  STATUS Logout() {return OK;}
  // Missing CreatePublisher, GetPublisher, CreateSubscriber, etc.
};

// Satisfies MessengerApp but not CommunicatorApp (no Setup, no client/service methods).
struct MessengerOnlyApp
{
  STATUS Login(const std::string &);
  STATUS Logout();
  template<typename T>
  STATUS CreatePublisher(const std::string &);
  template<typename T>
  std::shared_ptr<ProxyPublisher> GetPublisher(const std::string &);
  template<typename T>
  STATUS CreateSubscriber(const std::string &, Callback<T>);
  STATUS DeletePublisher(const std::string &);
  STATUS DeleteSubscriber(const std::string &);
};

// ─── Compile-time assertions (build fails if a concept is too permissive) ──────

// Publisher: accept
static_assert(Publisher<ProxyPublisher>);

// Publisher: reject
static_assert(!Publisher<int>);
static_assert(!Publisher<std::string>);
static_assert(!Publisher<PubNoMessageType>);
static_assert(!Publisher<PubNoPublish>);
static_assert(!Publisher<PubWrongReturn>);
static_assert(!Publisher<ProxyStreamerApp>);  // streamer ≠ publisher

// Client: accept
static_assert(Client<ProxyClient>);

// Client: reject
static_assert(!Client<int>);
static_assert(!Client<ClientNoServiceType>);
static_assert(!Client<ClientWrongWaitReturn>);
static_assert(!Client<ClientMissingAsync>);
static_assert(!Client<ProxyPublisher>);  // publisher ≠ client

// MessengerApp: accept
static_assert(MessengerApp<ProxyFullCommunicatorApp>);
static_assert(MessengerApp<MessengerOnlyApp>);

// MessengerApp: reject
static_assert(!MessengerApp<int>);
static_assert(!MessengerApp<ProxyPublisher>);   // publisher ≠ messenger app
static_assert(!MessengerApp<ProxyClient>);      // client ≠ messenger app
static_assert(!MessengerApp<ProxyStreamerApp>); // streamer app ≠ messenger app
static_assert(!MessengerApp<PartialMessenger>);

// CommunicatorApp: accept
static_assert(CommunicatorApp<ProxyFullCommunicatorApp>);

// CommunicatorApp: reject
static_assert(!CommunicatorApp<int>);
static_assert(!CommunicatorApp<ProxyPublisher>);
static_assert(!CommunicatorApp<ProxyStreamerApp>);
static_assert(!CommunicatorApp<MessengerOnlyApp>); // messenger alone ≠ communicator

// FullCommunicatorApp: accept
static_assert(FullCommunicatorApp<ProxyFullCommunicatorApp>);

// FullCommunicatorApp: reject
static_assert(!FullCommunicatorApp<MessengerOnlyApp>);  // no streaming
static_assert(!FullCommunicatorApp<ProxyStreamerApp>);  // no messaging

// ─── GTest: same checks with structured output ─────────────────────────────────

TEST(Publisher, AcceptsCorrectType)
{
  EXPECT_TRUE((Publisher<ProxyPublisher>));
}

TEST(Publisher, RejectsMissingMessageType)
{
  EXPECT_FALSE((Publisher<PubNoMessageType>));
}

TEST(Publisher, RejectsMissingPublish)
{
  EXPECT_FALSE((Publisher<PubNoPublish>));
}

TEST(Publisher, RejectsWrongReturnType)
{
  EXPECT_FALSE((Publisher<PubWrongReturn>));
}

TEST(Publisher, RejectsPrimitiveAndUnrelatedTypes)
{
  EXPECT_FALSE((Publisher<int>));
  EXPECT_FALSE((Publisher<std::string>));
  EXPECT_FALSE((Publisher<ProxyStreamerApp>));
}

TEST(Client, AcceptsCorrectType)
{
  EXPECT_TRUE((Client<ProxyClient>));
}

TEST(Client, RejectsMissingServiceType)
{
  EXPECT_FALSE((Client<ClientNoServiceType>));
}

TEST(Client, RejectsWrongWaitForServiceReturn)
{
  EXPECT_FALSE((Client<ClientWrongWaitReturn>));
}

TEST(Client, RejectsMissingAsyncRequest)
{
  EXPECT_FALSE((Client<ClientMissingAsync>));
}

TEST(Client, RejectsUnrelatedTypes)
{
  EXPECT_FALSE((Client<int>));
  EXPECT_FALSE((Client<ProxyPublisher>));
}

TEST(MessengerApp, AcceptsCorrectTypes)
{
  EXPECT_TRUE((MessengerApp<ProxyFullCommunicatorApp>));
  EXPECT_TRUE((MessengerApp<MessengerOnlyApp>));
}

TEST(MessengerApp, RejectsPartialImplementation)
{
  EXPECT_FALSE((MessengerApp<PartialMessenger>));
}

TEST(MessengerApp, RejectsUnrelatedTypes)
{
  EXPECT_FALSE((MessengerApp<int>));
  EXPECT_FALSE((MessengerApp<ProxyPublisher>));
  EXPECT_FALSE((MessengerApp<ProxyClient>));
  EXPECT_FALSE((MessengerApp<ProxyStreamerApp>));
}

TEST(CommunicatorApp, AcceptsCorrectType)
{
  EXPECT_TRUE((CommunicatorApp<ProxyFullCommunicatorApp>));
}

TEST(CommunicatorApp, RejectsMessengerOnlyApp)
{
  // Satisfies MessengerApp but is missing Setup + client/service methods.
  EXPECT_FALSE((CommunicatorApp<MessengerOnlyApp>));
}

TEST(CommunicatorApp, RejectsUnrelatedTypes)
{
  EXPECT_FALSE((CommunicatorApp<int>));
  EXPECT_FALSE((CommunicatorApp<ProxyPublisher>));
  EXPECT_FALSE((CommunicatorApp<ProxyStreamerApp>));
}

TEST(FullCommunicatorApp, AcceptsCorrectType)
{
  EXPECT_TRUE((FullCommunicatorApp<ProxyFullCommunicatorApp>));
}

TEST(FullCommunicatorApp, RejectsMessengerWithoutStreaming)
{
  EXPECT_FALSE((FullCommunicatorApp<MessengerOnlyApp>));
}

TEST(FullCommunicatorApp, RejectsStreamerWithoutMessaging)
{
  EXPECT_FALSE((FullCommunicatorApp<ProxyStreamerApp>));
}

int main(int argc, char ** argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
