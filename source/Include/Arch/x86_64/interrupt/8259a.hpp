#pragma once
#include <Lib/Types/Uefi.hpp>
PUBLIC namespace QuantumNEC::Architecture {
    PUBLIC class PIC8259A
    {
    private:
        using irq_t = Lib::Types::uint8_t;

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
        STATIC auto get_irr( void ) -> Lib::Types::uint16_t;
        STATIC auto get_isr( void ) -> Lib::Types::uint16_t;

    private:
        STATIC auto get_irq_reg( IN Lib::Types::int32_t ocw3 ) -> Lib::Types::uint16_t;
    };
}