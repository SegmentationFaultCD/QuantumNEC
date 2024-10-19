#pragma once
#include <kernel/interrupt/arch/x86_64/pit/8254.hpp>
#include <kernel/interrupt/arch/x86_64/pit/hpet.hpp>

PUBLIC namespace QuantumNEC::Kernel::x86_64 {
    PUBLIC class PIT :
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
        explicit PIT( VOID ) noexcept;

    public:
        virtual ~PIT( VOID ) noexcept;

    private:
    };
}