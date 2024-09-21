#include "include/memory.h"
namespace Kernel
{
	inline VOID* memset(VOID* Address, CHAR C, int64_t Count)
	{
		int32_t d0, d1;
		uint64_t tmp = C * 0x0101010101010101UL;
		__asm__	__volatile__	(	"cld	\n\t"
						"rep	\n\t"
						"stosq	\n\t"
						"testb	$4, %b3	\n\t"
						"je	1f	\n\t"
						"stosl	\n\t"
						"1:\ttestb	$2, %b3	\n\t"
						"je	2f\n\t"
						"stosw	\n\t"
						"2:\ttestb	$1, %b3	\n\t"
						"je	3f	\n\t"
						"stosb	\n\t"
						"3:	\n\t"
						:"=&c"(d0),"=&D"(d1)
						:"a"(tmp),"q"(Count),"0"(Count/8),"1"(Address)	
						:"memory"					
					);
		return Address;
	}

	inline int32_t memcmp(VOID* FirstPart, VOID* SecondPart, int64_t Count)
	{
		register int32_t __res;

		__asm__	__volatile__	(	"cld	\n\t"
						"repe	\n\t"
						"cmpsb	\n\t"
						"je	1f	\n\t"
						"movl	$1,	%%eax	\n\t"
						"jl	1f	\n\t"
						"negl	%%eax	\n\t"
						"1:	\n\t"
						:"=a"(__res)
						:"0"(0),"D"(FirstPart),"S"(SecondPart),"c"(Count)
						:
					);
		return __res;
	}

	inline VOID* memcpy(VOID* From, VOID* To, int64_t Num)
	{
		int32_t d0,d1,d2;
		__asm__ __volatile__	(	"cld	\n\t"
						"rep	\n\t"
						"movsq	\n\t"
						"testb	$4,%b4	\n\t"
						"je	1f	\n\t"
						"movsl	\n\t"
						"1:\ttestb	$2,%b4	\n\t"
						"je	2f	\n\t"
						"movsw	\n\t"
						"2:\ttestb	$1,%b4	\n\t"
						"je	3f	\n\t"
						"movsb	\n\t"
						"3:	\n\t"
						:"=&c"(d0),"=&D"(d1),"=&S"(d2)
						:"0"(Num/8),"q"(Num),"1"(To),"2"(From)
						:"memory"
					);
		return To;
	}
}