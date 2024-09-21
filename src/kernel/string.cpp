#include "include/string.h"
namespace Kernel
{
	inline CHAR* strcpy(CHAR* Dest, CHAR* Src)
	{
		__asm__	__volatile__	(	"cld	\n\t"
						"1:	\n\t"
						"lodsb	\n\t"
						"stosb	\n\t"
						"testb	%%al,	%%al	\n\t"
						"jne	1b	\n\t"
						:
						:"S"(Src),"D"(Dest)
						:
						
					);
		return 	Dest;
	}
	inline CHAR* strncpy(CHAR* Dest, CHAR* Src, int64_t Count)
	{
		__asm__	__volatile__	(	"cld	\n\t"
						"1:	\n\t"
						"decq	%2	\n\t"
						"js	2f	\n\t"
						"lodsb	\n\t"
						"stosb	\n\t"
						"testb	%%al,	%%al	\n\t"
						"jne	1b	\n\t"
						"rep	\n\t"
						"stosb	\n\t"
						"2:	\n\t"
						:
						:"S"(Src),"D"(Dest),"c"(Count)
						:					
					);
		return Dest;
	}

	inline CHAR* strcat(CHAR* Dest, CHAR* Src)
	{
		__asm__	__volatile__	(	"cld	\n\t"
						"repne	\n\t"
						"scasb	\n\t"
						"decq	%1	\n\t"
						"1:	\n\t"
						"lodsb	\n\t"
						"stosb	\n\r"
						"testb	%%al,	%%al	\n\t"
						"jne	1b	\n\t"
						:
						:"S"(Src),"D"(Dest),"a"(0),"c"(0xffffffff)
						:					
					);
		return Dest;
	}

	inline BOOL strcmp(CHAR* FirstPart, CHAR* SecondPart)
	{
		register BOOL __res;
		__asm__	__volatile__	(	"cld	\n\t"
						"1:	\n\t"
						"lodsb	\n\t"
						"scasb	\n\t"
						"jne	2f	\n\t"
						"testb	%%al,	%%al	\n\t"
						"jne	1b	\n\t"
						"xorl	%%eax,	%%eax	\n\t"
						"jmp	3f	\n\t"
						"2:	\n\t"
						"movl	$1,	%%eax	\n\t"
						"jl	3f	\n\t"
						"negl	%%eax	\n\t"
						"3:	\n\t"
						:"=a"(__res)
						:"D"(FirstPart),"S"(SecondPart)
						:					
					);
		return __res;
	}

	inline BOOL strncmp(CHAR* FirstPart, CHAR* SecondPart, int64_t Count)
	{	
		register BOOL __res;
		__asm__	__volatile__	(	"cld	\n\t"
						"1:	\n\t"
						"decq	%3	\n\t"
						"js	2f	\n\t"
						"lodsb	\n\t"
						"scasb	\n\t"
						"jne	3f	\n\t"
						"testb	%%al,	%%al	\n\t"
						"jne	1b	\n\t"
						"2:	\n\t"
						"xorl	%%eax,	%%eax	\n\t"
						"jmp	4f	\n\t"
						"3:	\n\t"
						"movl	$1,	%%eax	\n\t"
						"jl	4f	\n\t"
						"negl	%%eax	\n\t"
						"4:	\n\t"
						:"=a"(__res)
						:"D"(FirstPart),"S"(SecondPart),"c"(Count)
						:
					);
		return __res;
	}

	inline size_t strlen(CHAR* String)
	{
		register int32_t __res;
		__asm__	__volatile__	(	"cld	\n\t"
						"repne	\n\t"
						"scasb	\n\t"
						"notl	%0	\n\t"
						"decl	%0	\n\t"
						:"=c"(__res)
						:"D"(String),"a"(0),"0"(0xffffffff)
						:
					);
		return __res;
	}
}