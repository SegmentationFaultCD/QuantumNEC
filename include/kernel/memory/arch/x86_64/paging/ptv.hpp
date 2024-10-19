#pragma once
#include <lib/Uefi.hpp>
PUBLIC namespace QuantumNEC::Kernel::x86_64 {
    /**
     * @brief 将物理地址转换为线性地址
     */
    PUBLIC auto physical_to_virtual( VOID * address )->VOID *;
    /**
     * @brief 将线性地址转换为物理地址
     */
    PUBLIC auto virtual_to_physical( VOID * address )->VOID *;
    PUBLIC auto physical_to_virtual( uint64_t address ) -> VOID *;
    PUBLIC auto virtual_to_physical( uint64_t address ) -> VOID *;
}