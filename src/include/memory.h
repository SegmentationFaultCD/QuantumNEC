#ifndef _MEMORY_H_
#define _MEMORY_H_
#include "include/stdint.h"
#include "include/stdbool.h"
#include "include/stdio.h"
namespace Kernel
{
    /*  */

    inline VOID* memcpy(VOID* From, VOID* To, int64_t Num);

    /*  */

    inline int32_t memcmp(VOID* FirstPart, VOID* SecondPart, int64_t Count);

    /*  */
    
    inline VOID* memset(VOID* Address, CHAR C, int64_t Count);
}
#endif /* !_MEMORY_H_ */