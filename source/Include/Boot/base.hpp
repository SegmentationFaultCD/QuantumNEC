extern "C" {
#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiBootServicesTableLib.h>
}
#undef NULL
#define NULL nullptr
// #define DEBUG
#include <Boot/setup.hpp>
namespace QuantumNEC::Boot {
using int8_t = unsigned char;
using int16_t = unsigned short int;
using int32_t = unsigned int;
using int64_t = unsigned long long int;
using uint8_t = unsigned char;
using uint16_t = unsigned short int;
using uint32_t = unsigned int;
using uint64_t = unsigned long long int;
using uintptr_t = unsigned long long int;
using uchar_t = char16_t;
}     // namespace QuantumNEC::Boot
