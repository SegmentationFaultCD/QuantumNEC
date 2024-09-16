#pragma once
#include "Setup.hpp"
constexpr const auto TRUE { true };
constexpr const auto FALSE { false };
namespace QuantumNEC::Lib::Types {
#ifdef _C
typedef enum {
    true = 1, false = 0;
}
bool;
#else
using BOOL = bool;

#endif
}     // namespace QuantumNEC::Lib::Types