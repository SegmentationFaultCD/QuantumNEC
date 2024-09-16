#pragma once

#include <Arch/x86_64/interrupt/pit/8254.hpp>
#include <Arch/x86_64/interrupt/pit/hpet.hpp>
#include <Lib/Types/Uefi.hpp>

PUBLIC namespace QuantumNEC::Architecture {
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