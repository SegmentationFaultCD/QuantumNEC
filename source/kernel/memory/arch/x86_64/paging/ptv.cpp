#include <kernel/memory/arch/x86_64/paging/ptv.hpp>
#include <kernel/cpu/cpu.hpp>
using namespace QuantumNEC;
PUBLIC auto Kernel::x86_64::physical_to_virtual( VOID *address ) -> VOID * {
    return reinterpret_cast< VOID * >( (uint64_t)( address ) + __config.hhdm.offset );
}
/**
 * @brief 将线性地址转换为物理地址
 */
PUBLIC auto Kernel::x86_64::virtual_to_physical( VOID *address ) -> VOID * {
    return reinterpret_cast< VOID * >( (uint64_t)(address)-__config.hhdm.offset );
}
PUBLIC auto Kernel::x86_64::physical_to_virtual( uint64_t address ) -> VOID * {
    return reinterpret_cast< VOID * >( ( address ) + __config.hhdm.offset );
}

PUBLIC auto Kernel::x86_64::virtual_to_physical( uint64_t address ) -> VOID * {
    return reinterpret_cast< VOID * >( (address)-__config.hhdm.offset );
}