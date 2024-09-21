#ifndef _STDINT_H_
#define _STDINT_H_
namespace Kernel
{
    /* 无符号整形 */

    typedef unsigned char uint8_t;
    typedef unsigned short int uint16_t;
    typedef unsigned int uint32_t;
    typedef unsigned long long int uint64_t;

    typedef signed char BYTE;
    typedef signed short int SHORT;
    typedef signed int INT;
    typedef signed long long int LONG;
    /* 有符号整型 */

    typedef signed char int8_t;
    typedef signed short int int16_t;
    typedef signed int int32_t;
    typedef signed long long int int64_t;

    typedef unsigned char UBYTE;
    typedef unsigned short int USHORT;
    typedef unsigned int UINT;
    typedef unsigned long long int ULONG;

    /* 字符类型 */
    typedef unsigned char CHAR;
    /* 后期定义宽字符类型 */
    // typedef unsigned wchar_t WCHAR;

}
#endif /* !_STDINT_H_ */