#pragma once

#include <concepts>
#include <diagnostics/Status.hpp>
#include <communication/Communication.hpp>

namespace HBR::Manager
{

    template<typename T>
    concept IsManager =
    requires(T manager)
    {
        { manager.Prepare() } -> std::same_as<STATUS>;
        { manager.Cleanup() } -> std::same_as<STATUS>;
        { manager.Start() }   -> std::same_as<STATUS>;
        { manager.Stop() }    -> std::same_as<STATUS>;
        manager.Communicator;
        requires Communication::IsCommunicator<
            std::remove_cvref_t<decltype(manager.Communicator)>>;
    };

} // namespace HBR::Manager
