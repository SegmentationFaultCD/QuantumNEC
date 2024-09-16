#pragma once
#include "Setup.hpp"
namespace QuantumNEC::Lib::Types {
using UCHAR = unsigned char;
using CHAR = signed char;
using WCHAR = wchar_t;
using CHAR8 = char;
using CHAR16 = wchar_t;
using char_t = char;
#ifdef _u32
using uchar_t = char32_t;
#else
#ifdef _u16
using uchar_t = char16_t;
#else
#ifdef _u8
using uchar_t = char8_t;
#endif
#endif
#endif

}     // namespace QuantumNEC::Lib::Types