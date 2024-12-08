#pragma once
#include <lib/Uefi.hpp>

namespace QuantumNEC::Kernel::x86_64 {
class P8254 {
public:
    /**
     * @brief 初始化 8254 pit
     */
    explicit P8254( void ) noexcept;

    virtual ~P8254( void ) noexcept;
};
}     // namespace QuantumNEC::Kernel::x86_64