#ifndef _STRING_H_
#define _STRING_H_
#include "include/stdint.h"
#include "include/stdbool.h"
#include "include/stdio.h"
namespace Kernel
{
	/* 复制字符串 */
	inline CHAR* strcpy(CHAR* Dest, CHAR* Src);
	/* 复制字符串(安全版) */
	inline CHAR* strncpy(CHAR* Dest, CHAR* Src, int64_t Count);
	/* 拼接字符串 */
	inline CHAR* strcat(CHAR* Dest, CHAR* Src);
	/* 比较字符串 */
	inline BOOL strcmp(CHAR* FirstPart, CHAR* SecondPart);
	/* 比较字符串(安全版) */
	inline BOOL strncmp(CHAR* FirstPart, CHAR* SecondPart, int64_t Count);
	/* 测量字符串 */
	inline size_t strlen(CHAR* String);

	template <typename T>
	class string
	{
	public:  	// 函数区

	private: 	// 数据区

	protected:  // 保留区
	};
}

#endif /* !_STRING_H_ */