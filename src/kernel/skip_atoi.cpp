#include "include/printOut.h"

Kernel::size_t Kernel::skip_atoi(const char* str[])
{
    Kernel::int32_t i = 0;
    while (is_digit(**str))
    {
        i = i * 10 + *((*str)++) - '0';
    }
    return i;
}
