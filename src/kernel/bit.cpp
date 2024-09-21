#include "include/bit.h"
namespace Kernel
{
	inline uint64_t bit_set(uint64_t* addr, uint64_t nr)
	{
		return *addr | (1UL << nr);
	}

	inline uint64_t bit_get(uint64_t* addr, uint64_t nr)
	{
		return	*addr & (1UL << nr);
	}

	inline uint64_t bit_clean(uint64_t* addr, uint64_t nr)
	{
		return	*addr & (~(1UL << nr));
	}
}