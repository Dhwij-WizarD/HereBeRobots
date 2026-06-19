#include <communication/RosCommunicatorApp.hpp>
#include <rclcpp/rclcpp.hpp>

namespace HBR::Communication
{
    using namespace Diagnostics;

    STATUS RosCommunicatorApp::Setup(int argc, char** argv)
    {
        rclcpp::init(argc, argv);
        return OK;
    }

    RosCommunicatorApp::~RosCommunicatorApp()
    {
        Logout();
    }

    STATUS RosCommunicatorApp::Login(const std::string& id)
    {
        Logout();

        pNode = std::make_shared<rclcpp::Node>(id);
        if (!pNode) return ERROR;

        executor.add_node(pNode);
        {
            std::lock_guard lock(mtx);
            state = ThreadState::Running;
        }
        t = std::thread{&RosCommunicatorApp::Run, this};
        return OK;
    }

    STATUS RosCommunicatorApp::Logout()
    {
        {
            std::lock_guard lock(mtx);
            if (state == ThreadState::Idle) return OK;
        }

        executor.cancel();

        {
            std::unique_lock lock(mtx);
            cv.wait(lock, [this]{ return state == ThreadState::Idle; });
        }

        executor.remove_node(pNode);
        pNode.reset();
        if (t.joinable()) t.join();
        return OK;
    }

    void RosCommunicatorApp::Run()
    {
        executor.spin();

        {
            std::lock_guard lock(mtx);
            state = ThreadState::Idle;
        }
        cv.notify_all();
    }
}
