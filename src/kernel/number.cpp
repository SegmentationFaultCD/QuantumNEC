#include <Include/Kernel/Print/printOut.h>
namespace Types = QuantumNEC::Kernel::Types;

PRIVATE Types::TCHAR far* number(IN OUT Types::TCHAR far* str, IN Types::int64_t num, IN Types::int32_t base, IN Types::int32_t size, IN Types::int32_t precision, IN Types::int32_t type)
{
    Types::TCHAR ch, sign, tmp[50];
    CONST Types::TCHAR near*digits = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    Types::int32_t i;

    if (type & SMALL)
        digits = "0123456789abcdefghijklmnopqrstuvwxyz";
    if (type & LEFT)
        type &= ~ZEROPAD;
    if (base < 2 || base > 36)
        return 0;
    ch = (type & ZEROPAD) ? '0' : ' ';
    sign = 0;
    if (type & SIGN && num < 0)
    {
        sign = '-';
        num = -num;
    }
    else
    {
        sign = (type & PLUS) ? '+' : ((type & SPACE) ? ' ' : 0);
    }
    if (sign)
    {
        size--;
    }
    if (type & SPECIAL)
    {
        if (base == 16)
        {
            size -= 2;
        }
        else
        {
            if (base == 8)
            {
                size--;
            }
        }
    }
    i = 0;
    if (!num)
    {
        tmp[i++] = '0';
    }
    else
    {
        while (num)
        {
            tmp[i++] = digits[do_div(num, base)];
        }
    }
    if (i > precision)
    {
        precision = i;
    }
    size -= precision;
    if (!(type & (ZEROPAD + LEFT)))
    {
        while (size-- > 0)
        {
            *str++ = ' ';
        }
    }
    if (sign)
    {
        *str++ = sign;
    }
    if (type & SPECIAL)
    {
        if (base == 8)
        {
            *str++ = '0';
        }
        else if (base == 16)
        {
            *str++ = '0';
            *str++ = digits[33];
        }
    }
    if (!(type & LEFT))
    {
        while (size-- > 0)
        {
            *str++ = ch;
        }
    }

    while (i < precision--)
    {
        *str++ = '0';
    }
    while (i-- > 0)
    {
        *str++ = tmp[i];
    }
    while (size-- > 0)
    {
        *str++ = ' ';
    }
    return str;
}