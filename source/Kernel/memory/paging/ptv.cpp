#include <Kernel/memory/paging/ptv.hpp>
#include <Arch/Arch.hpp>
using namespace QuantumNEC;
PUBLIC auto Kernel::physical_to_virtual( VOID *address ) -> VOID * {
    return reinterpret_cast< VOID * >( (uint64_t)( address ) + Architecture::__config.hhdm.offset );
}
/**
 * @brief 将线性地址转换为物理地址
 */
PUBLIC auto Kernel::virtual_to_physical( VOID *address ) -> VOID * {
    return reinterpret_cast< VOID * >( (uint64_t)(address)-Architecture::__config.hhdm.offset );
}
PUBLIC auto Kernel::physical_to_virtual( uint64_t address ) -> VOID * {
    return reinterpret_cast< VOID * >( ( address ) + Architecture::__config.hhdm.offset );
}

PUBLIC auto Kernel::virtual_to_physical( uint64_t address ) -> VOID * {
    return reinterpret_cast< VOID * >( (address)-Architecture::__config.hhdm.offset );
}