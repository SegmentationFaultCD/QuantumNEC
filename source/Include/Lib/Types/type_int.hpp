#include "Setup.hpp"
namespace QuantumNEC::Lib::Types {
/* 无符号整形 */

using uint8_t = unsigned char;
using uint16_t = unsigned short int;
using uint32_t = unsigned int;
using uint64_t = unsigned long long int;
using uint128_t = unsigned __int128;
using uintptr_t = unsigned long long int;
/* 有符号整型 */
using int8_t = signed char;
using int16_t = signed short int;
using int32_t = signed int;
using int64_t = signed long long int;
using int128_t = signed __int128;
using intptr_t = signed long long int;

using byte_t = uint8_t;

/* 特殊类型 */

using size_t = unsigned long long int;

/* 大写类型 */

/* INTn 或 UINTn 写法 */

// 有符号整型

using INT8 = signed char;
using INT16 = signed short int;
using INT32 = signed int;
using INT64 = signed long long int;
using INT128 = __int128;
// 无符号整型

using UINT8 = unsigned char;
using UINT16 = unsigned short int;
using UINT32 = unsigned int;
using UINT64 = unsigned long long int;
using UINT128 = unsigned __int128;

using BYTE = byte_t;

/* 只是大写法 */

// 有符号整型

using SHORT = signed short int;
using INT = signed int;
using LONG = signed long long int;

// 无符号整型

using USHORT = unsigned short int;
using UINT = unsigned int;
using ULONG = unsigned long long int;

/* 习惯WINDOWS开发的人用的类型 */

using DWORD = unsigned int;
using QWORD = unsigned long long int;
using WORD = unsigned short int;

using time_t = unsigned int;

}     // namespace QuantumNEC::Lib::Types