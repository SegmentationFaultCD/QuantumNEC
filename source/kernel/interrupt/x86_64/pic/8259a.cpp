#include <kernel/cpu/cpu.hpp>
#include <kernel/interrupt/x86_64/pic/8259a.hpp>
#include <kernel/print.hpp>

namespace QuantumNEC::Kernel::x86_64 {
using namespace std;
PIC8259A::PIC8259A( void ) noexcept {
    this->enable_8259A_pic( );
    // 时间中断，键盘中断
    CPU::io_out8( PIC_M_DATA, 0xfc );
    CPU::io_out8( PIC_S_DATA, 0xff );
}
PIC8259A::~PIC8259A( void ) noexcept {
}
auto PIC8259A::eoi( IN irq_t irq ) -> void {
    if ( irq >= 0x28 ) {
        CPU::io_out8( PIC_S_CTRL, PIC_EOI );
    }
    CPU::io_out8( PIC_M_CTRL, PIC_EOI );
    return;
}
auto PIC8259A::disable_8259A_pic( void ) -> void {
    CPU::io_out8( PIC_M_DATA, 0xff );
    CPU::io_out8( PIC_S_DATA, 0xff );
}
auto PIC8259A::enable_8259A_pic( void ) -> void {
    CPU::io_out8( PIC_M_CTRL, 0x11 ); /* 边沿触发模式 */
    CPU::io_out8( PIC_M_DATA, 0x20 ); /* IRQ0-7由INT20-27接收 */
    CPU::io_out8( PIC_M_DATA, 0x04 ); /* PIC1由IRQ2连接*/
    CPU::io_out8( PIC_M_DATA, 0x01 ); /* 无缓冲区模式 */
    CPU::io_out8( PIC_S_CTRL, 0x11 ); /* 与上方类似 */
    CPU::io_out8( PIC_S_DATA, 0x28 ); /* IRQ8-15 INT28-2f */
    CPU::io_out8( PIC_S_DATA, 0x02 ); /* PIC1 IRQ2 */
    CPU::io_out8( PIC_S_DATA, 0x01 ); /* 无缓冲区模式 */
}
auto PIC8259A::irq_set_mask( IN irq_t irq ) -> void {
    uint16_t port { };
    if ( irq < 8 ) {
        port = PIC_M_DATA;
    }
    else {
        port = PIC_S_DATA;
        irq -= 8;
    }
    CPU::io_out8( port, CPU::io_in8( port ) | ( 1 << irq ) );
}
auto PIC8259A::irq_clear_mask( IN irq_t irq ) -> void {
    uint16_t port { };
    if ( irq < 8 ) {
        port = PIC_M_DATA;
    }
    else {
        port = PIC_S_DATA;
        irq -= 8;
    }
    CPU::io_out8( port, CPU::io_in8( port ) & ~( 1 << irq ) );
}
auto PIC8259A::get_irq_reg( IN int32_t ocw3 ) -> uint16_t {
    CPU::io_out8( PIC_M_COMMAND, ocw3 );
    CPU::io_out8( PIC_S_COMMAND, ocw3 );
    return ( CPU::io_in8( PIC_S_COMMAND ) << 8 ) | CPU::io_in8( PIC_M_COMMAND );
}
auto PIC8259A::get_irr( void ) -> uint16_t {
    return get_irq_reg( PIC_READ_IRR );
}
auto PIC8259A::get_isr( void ) -> uint16_t {
    return get_irq_reg( PIC_READ_ISR );
}
}     // namespace QuantumNEC::Kernel::x86_64
