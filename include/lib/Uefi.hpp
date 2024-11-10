#pragma once
#include "deflib.hpp"

#include "../source/boot/limine/limine.h"

#define OUT
#define IN
#ifndef NULL
#define NULL nullptr
#endif
namespace QuantumNEC {

using UCHAR = unsigned char;
using CHAR = signed char;
using WCHAR = wchar_t;
using CHAR8 = char;
using CHAR16 = wchar_t;
using char_t = char;
using uchar_t = char16_t;
enum : bool {
    TRUE = true,
    FALSE = false
};
using double32_t = float;
using double64_t = double;
using double128_t = long double;

using BOOL = bool;

using UINTN = uint64_t;
using EfiPhysicalAddress = uint64_t;
using EfiVirtualAddress = uint64_t;
using PhysicalAddress = uint64_t;
using VirtualAddress = uint64_t;

using SystemStatus = UINTN;
inline constexpr auto SYSTEM_SUCCESS { (SystemStatus)0 };

template < typename R, typename... Args >
using FuncPtr = R ( * )( Args... );
template < typename R, typename C, typename... Args >
using ClassFuncPtr = R ( C::* )( Args... );
template < typename T >
using Ptr = T *;
template < typename T >
using L_Ref = T &;
template < typename T >
using R_Ref = T &&;
/* 无符号整形 */

using uint8_t = unsigned char;
using uint16_t = unsigned short int;
using uint32_t = unsigned int;
using uint64_t = unsigned long int;
using uint128_t = unsigned __int128;
using uintptr_t = unsigned long int;
/* 有符号整型 */
using int8_t = signed char;
using int16_t = signed short int;
using int32_t = signed int;
using int64_t = signed long int;
using int128_t = signed __int128;
using intptr_t = signed long int;

using byte_t = uint8_t;

using size_t = unsigned long int;

/* 大写类型 */

/* INTn 或 UINTn 写法 */

// 有符号整型

using INT8 = signed char;
using INT16 = signed short int;
using INT32 = signed int;
using INT64 = signed long int;
using INT128 = __int128;
// 无符号整型

using UINT8 = unsigned char;
using UINT16 = unsigned short int;
using UINT32 = unsigned int;
using UINT64 = unsigned long int;
using UINT128 = unsigned __int128;

using BYTE = byte_t;

/* 只是大写法 */

// 有符号整型

using SHORT = signed short int;
using INT = signed int;
using LONG = signed long int;

// 无符号整型

using USHORT = unsigned short int;
using UINT = unsigned int;
using ULONG = unsigned long int;

/* 习惯WINDOWS开发的人用的类型 */

using DWORD = unsigned int;
using QWORD = unsigned long int;
using WORD = unsigned short int;

using time_t = unsigned int;
struct _packed FontConfig
{
    uint64_t length;
    VOID *frame;
};
struct BootConfig
{
    limine_framebuffer graphics_data;
    limine_memmap_response memory_map;
    limine_rsdp_response acpi_table;
    FontConfig font_frame;
    limine_hhdm_response hhdm;
    limine_smp_response smp;
    limine_paging_mode_response paging_mode;
    limine_module_response modules;
};

}     // namespace QuantumNEC