#pragma once
#include <lib/Uefi.hpp>
namespace QuantumNEC::Kernel::x86_64 {
/**
 * @brief 将物理地址转换为线性地址
 */
auto physical_to_virtual( void *address ) -> void *;
/**
 * @brief 将线性地址转换为物理地址
 */
auto virtual_to_physical( void *address ) -> void *;
auto physical_to_virtual( uint64_t address ) -> void *;
auto virtual_to_physical( uint64_t address ) -> void *;
}     // namespace QuantumNEC::Kernel::x86_64