#pragma once
#include <communication/Concepts.hpp>
#include <rclcpp/rclcpp.hpp>
#include <thread>
#include <mutex>
#include <condition_variable>

namespace HBR::Communication
{
    class RosCommunicatorApp
    {
    public:
        RosCommunicatorApp() = default;
        ~RosCommunicatorApp();

        STATUS Setup(int argc, char** argv);

        STATUS Login(const std::string& id);
        STATUS Logout();

        template<typename MSG>
        STATUS CreatePublisher(const std::string& name);
        template<typename MSG>
        STATUS CreateSubscriber(const std::string& name, Callback<MSG>);

        STATUS DeletePublisher(const std::string& name);
        STATUS DeleteSubscriber(const std::string& name);

        template<typename REQ, typename RES>
        STATUS CreateClient(const std::string& name);
        template<typename REQ, typename RES>
        STATUS CreateService(const std::string& name, ResponderCallback<REQ, RES> rcb);

        STATUS DeleteClient(const std::string& name);
        STATUS DeleterService(const std::string& name);

        void Run();

    private:
        enum class ThreadState { Idle, Running };

        std::thread t;
        std::mutex mtx;
        std::condition_variable cv;
        ThreadState state { ThreadState::Idle };

        rclcpp::Node::SharedPtr pNode;
        rclcpp::executors::MultiThreadedExecutor executor;
        rclcpp::CallbackGroup::SharedPtr sharedGroup;
        rclcpp::CallbackGroup::SharedPtr clientGroup;
    };

    static_assert(CommunicatorApp<RosCommunicatorApp>);
} // namespace HBR::Communication
