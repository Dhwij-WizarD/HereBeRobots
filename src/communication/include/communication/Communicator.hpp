#pragma once
#include <communication/Concepts.hpp>
#include <communication/RosCommunicatorApp.hpp>
#include <variant>

namespace HBR::Communication
{
using Communicator_VT = std::variant<RosCommunicatorApp>;     // suffix _VT = Variant Type
}
