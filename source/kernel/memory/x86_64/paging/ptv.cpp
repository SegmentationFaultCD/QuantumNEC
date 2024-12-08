#include <kernel/cpu/cpu.hpp>
#include <kernel/memory/x86_64/paging/ptv.hpp>
using namespace QuantumNEC;
auto Kernel::x86_64::physical_to_virtual( void *address ) -> void * {
    return reinterpret_cast< void * >( (uint64_t)( address ) + __config.hhdm.offset );
}
/**
 * @brief 将线性地址转换为物理地址
 */
auto Kernel::x86_64::virtual_to_physical( void *address ) -> void * {
    return reinterpret_cast< void * >( (uint64_t)(address)-__config.hhdm.offset );
}
auto Kernel::x86_64::physical_to_virtual( uint64_t address ) -> void * {
    return reinterpret_cast< void * >( ( address ) + __config.hhdm.offset );
}

auto Kernel::x86_64::virtual_to_physical( uint64_t address ) -> void * {
    return reinterpret_cast< void * >( (address)-__config.hhdm.offset );
}