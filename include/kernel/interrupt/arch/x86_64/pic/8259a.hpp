#pragma once
#include <lib/Uefi.hpp>
PUBLIC namespace QuantumNEC::Kernel::x86_64 {
    PUBLIC class PIC8259A
    {
    private:
        using irq_t = uint64_t;

    public:
        explicit PIC8259A( VOID ) noexcept;

    public:
        virtual ~PIC8259A( VOID ) noexcept;

    public:
        STATIC auto eoi( IN CONST irq_t irq ) -> VOID;
        STATIC auto disable_8259A_pic( VOID ) -> VOID;
        STATIC auto enable_8259A_pic( VOID ) -> VOID;
        STATIC auto irq_set_mask( IN irq_t irq ) -> VOID;
        STATIC auto irq_clear_mask( IN irq_t irq ) -> VOID;
        STATIC auto get_irr( void ) -> uint16_t;
        STATIC auto get_isr( void ) -> uint16_t;

    private:
        STATIC auto get_irq_reg( IN int32_t ocw3 ) -> uint16_t;
    };
}