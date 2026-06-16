//=============================================================================
/// \file		DiagnosticSink.h
///
/// A sink for collecting all diagnostic logs
///
/// \date		16 Jun 2026
/// \author		PiyushMahamuni
//=============================================================================
#pragma once

#include <diagnostics/Status.hpp>
#include <fstream>

namespace Diagnostics
{
    class DiagnosticSink
    {
    public:
        DiagnosticSink() = default;
        STATUS Init(const std::string& file) noexcept;
    protected:
    private:
        std::fstream file;
    };
}