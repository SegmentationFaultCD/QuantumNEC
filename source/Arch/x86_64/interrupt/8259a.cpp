#include "Arch/x86_64/interrupt/8259a.hpp"
#include <Arch/x86_64/platform/platform.hpp>
PUBLIC namespace QuantumNEC::Architecture {
    PIC8259A::PIC8259A( VOID ) noexcept {
        this->enable_8259A_pic( );
        // 时间中断，键盘中断
        CPUs::io_out8( PIC_M_DATA, 0xfc );
        CPUs::io_out8( PIC_S_DATA, 0xff );
    }
    PIC8259A::~PIC8259A( VOID ) noexcept {
    }
    auto PIC8259A::eoi( IN CONST irq_t irq )->VOID {
        if ( irq >= 0x28 ) {
            CPUs::io_out8( PIC_S_CTRL, PIC_EOI );
        }
        CPUs::io_out8( PIC_M_CTRL, PIC_EOI );
        return;
    }
    auto PIC8259A::disable_8259A_pic( VOID )->VOID {
        CPUs::io_out8( PIC_M_DATA, 0xff );
        CPUs::io_out8( PIC_S_DATA, 0xff );
    }
    auto PIC8259A::enable_8259A_pic( VOID )->VOID {
        CPUs::io_out8( PIC_M_CTRL, 0x11 ); /* 边沿触发模式 */
        CPUs::io_out8( PIC_M_DATA, 0x20 ); /* IRQ0-7由INT20-27接收 */
        CPUs::io_out8( PIC_M_DATA, 0x04 ); /* PIC1由IRQ2连接*/
        CPUs::io_out8( PIC_M_DATA, 0x01 ); /* 无缓冲区模式 */
        CPUs::io_out8( PIC_S_CTRL, 0x11 ); /* 与上方类似 */
        CPUs::io_out8( PIC_S_DATA, 0x28 ); /* IRQ8-15 INT28-2f */
        CPUs::io_out8( PIC_S_DATA, 0x02 ); /* PIC1 IRQ2 */
        CPUs::io_out8( PIC_S_DATA, 0x01 ); /* 无缓冲区模式 */
    }
    auto PIC8259A::irq_set_mask( IN irq_t irq )->VOID {
        Lib::Types::uint16_t port { };
        if ( irq < 8 ) {
            port = PIC_M_DATA;
        }
        else {
            port = PIC_S_DATA;
            irq -= 8;
        }
        CPUs::io_out8( port, CPUs::io_in8( port ) | ( 1 << irq ) );
    }
    auto PIC8259A::irq_clear_mask( IN irq_t irq )->VOID {
        Lib::Types::uint16_t port { };
        if ( irq < 8 ) {
            port = PIC_M_DATA;
        }
        else {
            port = PIC_S_DATA;
            irq -= 8;
        }
        CPUs::io_out8( port, CPUs::io_in8( port ) & ~( 1 << irq ) );
    }
    auto PIC8259A::get_irq_reg( IN Lib::Types::int32_t ocw3 )->Lib::Types::uint16_t {
        CPUs::io_out8( PIC_M_COMMAND, ocw3 );
        CPUs::io_out8( PIC_S_COMMAND, ocw3 );
        return ( CPUs::io_in8( PIC_S_COMMAND ) << 8 ) | CPUs::io_in8( PIC_M_COMMAND );
    }
    auto PIC8259A::get_irr( void )->Lib::Types::uint16_t {
        return get_irq_reg( PIC_READ_IRR );
    }
    auto PIC8259A::get_isr( void )->Lib::Types::uint16_t {
        return get_irq_reg( PIC_READ_ISR );
    }
}