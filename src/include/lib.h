#ifndef _LIB_H_
#define _LIB_H_

#define container_of(ptr, type, member)										\
(																			\
	{																		\
	typeof(((type *)0)->member) * p = (ptr);								\
	(type *)((unsigned long)p - (unsigned long)&(((type *)0)->member));		\
	}																		\
)

#define port_insw(port, buffer, nr)	\
__asm__ __volatile__("cld;rep;insw;mfence;"::"d"(port),"D"(buffer),"c"(nr):"memory")

#define port_outsw(port, buffer, nr)	\
__asm__ __volatile__("cld;rep;outsw;mfence;"::"d"(port),"S"(buffer),"c"(nr):"memory")


#define sti() 			__asm__ __volatile__ ("sti	\n\t":::"memory")
#define cli()	 		__asm__ __volatile__ ("cli	\n\t":::"memory")
#define nop() 			__asm__ __volatile__ ("nop	\n\t")
#define io_mfence() 	__asm__ __volatile__ ("mfence	\n\t":::"memory")



#endif // !_LIB_H_
