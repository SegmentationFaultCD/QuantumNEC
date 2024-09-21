#include "include/printOut.h"
namespace Kernel
{
    int32_t vsprintf(CHAR* buf, CONST CHAR *fmt, va_list args)
    {
        CHAR* str1, *str2;
        int32_t flags,
                field_width,
                precision,
                len,
                i,
                qualifier;
        for (str1 = buf; *fmt; ++fmt)
        {
            if(*fmt != '%') // 如果不是'%'则认为是一个可显示字符
            {
                *(++str1) = *fmt;
                continue;
            }
            flags = 0;
            repeat:
            ++fmt;
            switch(*fmt) // 如果是则判断后面跟的格式符
            {
            case '-':
                flags |= LEFT;
                goto repeat; 
            case '+':
                flags |= PLUS;
                goto repeat;
            case ' ':
                flags |= SPACE;
                goto repeat;
            case '#':
                flags |= SPECIAL;
                goto repeat;
            case '0':
                flags |= ZEROPAD;
                goto repeat;
            }
            // 提取出后续字符串里的数字并转换为数值, 用来表示数据区域的宽度
            field_width = -1;
            if (is_digit(*fmt)) {
                field_width = skip_atoi(&fmt);
            }
            // 如果不是数字而是'*', 那么数据区域将由可变参数提供
            else if (*fmt == '*')
            {
                ++fmt;
                field_width = va_arg(args, int32_t);
                if (field_width < 0)
                {
                    field_width = -field_width;
                    flags |= LEFT;
                }
            }
            // 提取出显示数据的精度
            precision = -1;
            if (*fmt == '.')
            {
                ++fmt;
                if (is_digit(*fmt)) {
                    precision = skip_atoi(&fmt);
                }
                else if (*fmt == '*')
                {
                    ++fmt;
                    precision = va_arg(args, int32_t);
                    if (precision < 0) {
                        precision = 0;
                    }
                }
            }
            // 检查显示数据的规格
            qualifier = -1;
            if
            (
                *fmt == 'h' ||
                *fmt == 'l' ||
                *fmt == 'L' ||
                *fmt == 'Z'
            )
            {
                qualifier = *fmt;
                ++fmt;
            }
            switch (*fmt)
            {
            case 'c':
                if (!(flags & LEFT)) {
                    while (--field_width > 0) {
                        *(++str1) = ' ';
                    }
                }
                *(++str1) = (CHAR)va_arg(args, int32_t);
                while (--field_width > 0) {
                    *(++str1) = ' ';
                }
                break;
            case 's':
                str2 = va_arg(args, CHAR*);
                if (!str2) {
                    str2 = '\0';
                }
                len = strlen(str2);
                if (precision < 0) {
                    precision = len;
                }
                else if (len > precision) {
                    len = precision;
                }
                if (!(flags & LEFT)) {
                    while (len < --field_width) {
                        *(++str1) = ' ';
                    }
                }
                for (i = 0; i < len; ++i) {
                    *(++str1) = *(++str2);
                }
                while (len < --field_width) {
                    *(++str1) = ' ';
                }
                break;
            case 'o':
                if (qualifier == 'l') 
                {
                    str1 = number
                    (
                        str1,
                        va_arg(args, uint64_t),
                        8,
                        field_width,
                        precision,
                        flags
                    );
                }
                else 
                {
                    str1 = number
                    (
                        str1,
                        va_arg(args, uint32_t),
                        8,
                        field_width,
                        precision,
                        flags
                    );
                }
                break;
            case 'p':
					if (field_width == -1)
					{
						field_width = 2 * sizeof(VOID*);
						flags |= ZEROPAD;
					}

					str1 = number
                    (
                        str1,
                        (uint64_t)va_arg(args, VOID*),
                        16,
                        field_width,
                        precision,
                        flags
                    );
					break;
            }

        }
    }
}