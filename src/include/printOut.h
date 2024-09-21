#ifndef _PRINTK_H_
#define _PRINTK_H_

#include <stdarg.h>

#include "include/font.h"
#include "include/linkage.h"
#include "include/stdint.h"
#include "include/stdio.h"
#include "include/string.h"

#define ZEROPAD 1
#define SIGN 2
#define PLUS 4
#define SPACE 8
#define LEFT 16
#define SPECIAL 32
#define SMALL 64

#define is_digit(c) ((c) >= '0' && (c) <= '9')

#define WHITE 0x00ffffff  // 白
#define BLACK 0x00000000  // 黑
#define RED 0x00ff0000	  // 红
#define ORANGE 0x00ff8000 // 橙
#define YELLOW 0x00ffff00 // 黄
#define GREEN 0x0000ff00  // 绿
#define BLUE 0x000000ff	  // 蓝
#define INDIGO 0x0000ffff // 靛
#define PURPLE 0x008000ff // 紫

#define do_div(n, base)                       \
	({                                        \
		int __res;                            \
		__asm__("divq %%rcx"                  \
				: "=a"(n), "=d"(__res)        \
				: "0"(n), "1"(0), "c"(base)); \
		__res;                                \
	})

extern Kernel::CHAR Ascii[256][16]; /* 外部引用ascii库 */

namespace Kernel
{
	extern CHAR Ascii[256][16];

	CHAR buffer[4096] = {0};

	typedef struct
	{
		int32_t XResolution;
		int32_t YResolution;

		int32_t XPosition;
		int32_t YPosition;

		int32_t XCharSize;
		int32_t YCharSize;

		uint32_t *FB_addr;
		uint64_t FB_length;
	} Position /* 存储屏幕信息 */;

	Position Pos;
	// 打印字符
	VOID putChar(uint32_t *fb, int32_t Xsize, int32_t x, int32_t y, uint32_t FRcolor, uint32_t BKcolor, uint8_t font);

	size_t skip_atoi(CONST CHAR *s[]);

	static CHAR *number(CHAR *str, int64_t num, int32_t base, int32_t size, int32_t precision, int32_t type);
	// 格式化字符串的显示工作
	int32_t vsprintf(CHAR *buf, CONST CHAR *fmt, va_list args);
	// 显示彩色字符函数
	int32_t printk(uint32_t FRcolor, uint32_t BKcolor, CONST CHAR *fmt, ...);
	
	
} // namespace Kernel
#endif /* !_PRINTK_H_ */
