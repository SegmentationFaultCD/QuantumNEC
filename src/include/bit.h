#ifndef _BIT_H_
#define _BIT_H_
#include "include/stdint.h"
namespace Kernel
{
    inline uint64_t bit_set(uint64_t* addr, uint64_t nr);

    inline uint64_t bit_get(uint64_t* addr, uint64_t nr);

    inline uint64_t bit_clean(uint64_t* addr, uint64_t nr);
}
#endif /* !_BIT_H_ */