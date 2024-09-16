#pragma once
#include <Lib/Types/Uefi.hpp>
#include <Lib/Types/type_bool.hpp>
#include <Arch/x86_64/interrupt/8259a.hpp>
#include <Arch/x86_64/interrupt/apic.hpp>
#include <Arch/x86_64/interrupt/entry/entry.hpp>
#include <Arch/x86_64/interrupt/pit/pit.hpp>

PUBLIC namespace QuantumNEC::Architecture {
    PUBLIC class Interrupt :
#ifndef APIC
        public PIC8259A,
#else
        public Apic,
#endif
        InterruptEntryRegister
    {
    public:
        enum class InterruptStatus : Lib::Types::BOOL {
            INTERRUPT_DISABLE = FALSE, /* 关闭 */
            INTERRUPT_ENABLE = TRUE    /* 打开 */
        };

    public:
        explicit Interrupt( VOID ) noexcept;

    public:
        virtual ~Interrupt( VOID ) noexcept;

    public:
        /**
         * @brief 开中断
         */
        STATIC auto enable_interrupt( VOID ) -> InterruptStatus;
        /**
         * @brief 关中断
         */
        STATIC auto disable_interrupt( VOID ) -> InterruptStatus;
        /**
         * @brief 设置中断状态
         */
        STATIC auto set_interrupt( IN Lib::Types::L_Ref< CONST InterruptStatus > status ) -> InterruptStatus;
        /**
         * @brief 获取中断状态
         */
        STATIC auto get_interrupt( VOID ) -> InterruptStatus;
    };
}