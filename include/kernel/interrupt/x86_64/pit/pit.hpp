#pragma once
#include <kernel/interrupt/x86_64/pit/8254.hpp>
#include <kernel/interrupt/x86_64/pit/hpet.hpp>

namespace QuantumNEC::Kernel::x86_64 {
class PIT :
#ifndef APIC
    P8254     // 初始化8253_pit
#else
    Hpet     // 初始化Hpet
#endif
{
public:
    /**
     * @brief 初始化 PIT 8253 or HEPT
     */
    explicit PIT( void ) noexcept;

public:
    virtual ~PIT( void ) noexcept;

private:
};
}     // namespace QuantumNEC::Kernel::x86_64