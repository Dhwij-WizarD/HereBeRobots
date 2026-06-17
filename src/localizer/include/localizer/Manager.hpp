//=============================================================================
/// \file		localizer/Manager.hpp
///
/// Manages the localizations controllers. Acts as ros2 orchestrator.
///
/// \date		16 Jun 2026
/// \author		PiyushMahamuni
//=============================================================================

#pragma once
#include <diagnostics/Status.hpp>

namespace HBR::Localizer
{
    using namespace Diagnostics;

    class final Manager
    {
    public:
        Manager() = default;
        ~Manager() = default;
        void Spin();
        STATUS Init();
        STATUS Deinit();
    private:
        rclcpp::Node::SharedPtr pNode;
    };
}