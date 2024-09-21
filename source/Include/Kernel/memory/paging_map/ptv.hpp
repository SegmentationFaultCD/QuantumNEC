#pragma once
#include <Lib/Uefi.hpp>
PUBLIC namespace QuantumNEC::Kernel {
    /**
     * @brief 将物理地址转换为线性地址
     */
    PUBLIC auto physical_to_virtual( auto address ) -> VOID *;
    /**
     * @brief 将线性地址转换为物理地址
     */
    PUBLIC auto virtual_to_physical( auto address ) -> VOID *;
}