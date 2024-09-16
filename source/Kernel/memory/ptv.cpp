#include <Kernel/memory/ptv.hpp>
#include <Arch/Arch.hpp>
using namespace QuantumNEC;

PUBLIC auto Kernel::physical_to_virtual( auto address ) -> VOID * {
    return reinterpret_cast< VOID * >( (uint64_t)( address ) + Architecture::__config.hhdm.offset );
}
/**
 * @brief 将线性地址转换为物理地址
 */
PUBLIC auto Kernel::virtual_to_physical( auto address ) -> VOID * {
    return reinterpret_cast< VOID * >( (uint64_t)(address)-Architecture::__config.hhdm.offset );
}