#include "include/printOut.h"
namespace Kernel
{
    int32_t printk(uint32_t FRcolor, uint32_t BKcolor, CONST CHAR* fmt,...)
    {
        int32_t i = 0;
        int32_t count = 0;
        int32_t line = LINEEOF;
        va_list args;
        va_start(args, fmt);
        i = vsprintf(buffer, fmt, args);
        va_end(args);
        for (count = 0; count < i || line ; ++count)
        {
            if (line > LINEEOF)
            {
                --count;
                goto L_tab;
            }
            if ((CHAR)*(buffer + count) == '\n') // 判断是字符否为\n
		    {
			    ++(Pos.YPosition);
			    Pos.XPosition = LINEEOF;
                // 如果是，将光标行数加1, 列数设为0
		    }
            else if ((CHAR)*(buffer + count) == '\b') // 判断是字符否为\b
            {
                --(Pos.XPosition);
                if (Pos.XPosition < LINEEOF)
                {
                    Pos.XPosition = (Pos.XResolution / Pos.XCharSize - 1) * Pos.XCharSize;
                    --(Pos.YPosition);
                    if (Pos.YPosition < LINEEOF) {
                        Pos.YPosition = (Pos.YResolution / Pos.YCharSize) * Pos.YCharSize;
                    }
                }
                putChar
                (    
                    Pos.FB_addr,
                    Pos.XResolution,
                    Pos.XPosition * Pos.XCharSize,
                    Pos.YPosition * Pos.YCharSize,
                    FRcolor,
                    BKcolor,
                    ' '
                );
                // 如果是, 调用putChar函数打印空格来覆盖"\b", 并调整位置
            }
            else if ((CHAR)*(buffer + count) == '\t') // 判断是字符否为\t
            {
                line = ((Pos.XPosition + 4) & ~(4 - 1)) - Pos.XPosition; // 表示一个制表符需要4个字符位置
                L_tab:
                --line;
                putChar
                (    
                    Pos.FB_addr,
                    Pos.XResolution,
                    Pos.XPosition * Pos.XCharSize,
                    Pos.YPosition * Pos.YCharSize,
                    FRcolor,
                    BKcolor,
                    ' '
                );
                ++(Pos.XPosition);
                // 如果是\t, 那么计算一个制表符所需要的空格数, 并填充空格
            }
            else { // 将字符打印在屏幕上
                putChar
                (    
                    Pos.FB_addr,
                    Pos.XResolution,
                    Pos.XPosition * Pos.XCharSize,
                    Pos.YPosition * Pos.YCharSize,
                    FRcolor,
                    BKcolor,
                    (CHAR)*(buffer + count)
                );
            }
            // 结尾部分
            if (Pos.XPosition >= (Pos.XResolution / Pos.XCharSize))
            {
                ++(Pos.YPosition);
                Pos.XPosition = LINEEOF;
            }
            if (Pos.YPosition >= (Pos.YResolution / Pos.YCharSize))
            {
                Pos.YPosition = LINEEOF;
            }
        }
        return i;
    }
}