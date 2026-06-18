//=============================================================================
/// \file		Status.h
///
/// Defines status i.e. error codes to be used in the HBR stack.
///
/// \date		16 Jun 2026
/// \author		PiyushMahamuni
//=============================================================================
#pragma once
#include <unordered_map>
#include <string>
#include <tl/expected.hpp>


namespace HBR::Diagnostics
{
    enum STATUS : int32_t
    {
        OK               = 0,
        WARN             = 1,
        ERROR            = 2,
        TIMEOUT          = 3,
        UNINITIALIZED    = 4,
        BUSY             = 5,
        COMM_FAIL        = 6,
        DATA_INVALID     = 7,
        OVERHEAT         = 8,
        LOW_RESOURCE     = 9,
        INIT_FAILED      = 10,
        ENABLE_FAILED    = 11,
        DISABLE_FAILED   = 12,
        DEINIT_FAILED    = 13,
        CALIBRATE_FAILED = 14,
        START_FAILED     = 15,
        CONFIG_FAILED    = 16,
        NODE_OFFLINE     = 17
    };

    static const std::unordered_map<int32_t, std::string> STATUS_LUT =
    {
        {OK               , "STATUS_OK"},
        {WARN             , "WARNING"},
        {TIMEOUT          , "TIMEOUT"},
        {UNINITIALIZED    , "UNINITIALIZED"},
        {BUSY             , "BUSY"},
        {COMM_FAIL        , "COMM FAILURE"},
        {DATA_INVALID     , "INVALID DATA"},
        {OVERHEAT         , "OVERHEAT"},
        {LOW_RESOURCE     , "LOW RESOURCES"},
        {INIT_FAILED      , "INIT FAILED"},
        {ENABLE_FAILED    , "ENABLE FAILED"},
        {DISABLE_FAILED   , "DISABLE FAILED"},
        {DEINIT_FAILED    , "DEINIT FAILED"},
        {CALIBRATE_FAILED , "CALIBRATE FAILED"},
        {START_FAILED     , "START FAILED"},
        {CONFIG_FAILED    , "CONFIG FAILED"},
        {NODE_OFFLINE     , "NODE IS OFFLINE"}
    };

    struct Log
    {
        Log () : status(OK), message("") {}
        Log(STATUS status, std::string message) : status(status), message(message) {}
        STATUS status {OK};
        std::string message {STATUS_LUT.at(OK)};
    };

    template<typename T>
    using Result = tl::expected<T, Log>;
    using Failure = tl::unexpected<Log>;
}