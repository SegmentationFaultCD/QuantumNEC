#include <kernel/cpu/cpu.hpp>
#include <kernel/interrupt/arch/x86_64/pit/8254.hpp>
#include <kernel/print.hpp>
#include <lib/spin_lock.hpp>
PUBLIC namespace QuantumNEC::Kernel::x86_64 {
    using namespace std;
    P8254 ::P8254( VOID ) noexcept {
        /* 往控制字寄存器端口 0x43 中写入控制字 */
        CPU::io_out8( PIT_CONTROL_PORT, static_cast< uint8_t >( COUNTER0_NO << 6 | READ_WRITE_LATCH << 4 | COUNTER_MODE << 1 ) );
        /* 先写入 counter_value 的低 8 位 */
        CPU::io_out8( CONTRER0_PORT, static_cast< uint8_t >( COUNTER0_VALUE ) );
        /* 再写入 counter_value 的高 8 位 */
        CPU::io_out8( CONTRER0_PORT, static_cast< uint8_t >( COUNTER0_VALUE ) >> 8 );

        // println< ostream::HeadLevel::OK >( "Initialize the 8254 programmable interval timer." );
    }
    P8254::~P8254( VOID ) noexcept {
    }
}
