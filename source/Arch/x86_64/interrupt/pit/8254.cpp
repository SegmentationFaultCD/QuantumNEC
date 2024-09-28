#include <Arch/x86_64/platform/platform.hpp>
#include <Kernel/print.hpp>
PUBLIC namespace QuantumNEC::Architecture {
    using namespace std;
    P8254 ::P8254( VOID ) noexcept {
        /* 往控制字寄存器端口 0x43 中写入控制字 */
        CPUs::io_out8( PIT_CONTROL_PORT, static_cast< uint8_t >( COUNTER0_NO << 6 | READ_WRITE_LATCH << 4 | COUNTER_MODE << 1 ) );
        /* 先写入 counter_value 的低 8 位 */
        CPUs::io_out8( CONTRER0_PORT, static_cast< uint8_t >( COUNTER0_VALUE ) );
        /* 再写入 counter_value 的高 8 位 */
        CPUs::io_out8( CONTRER0_PORT, static_cast< uint8_t >( COUNTER0_VALUE ) >> 8 );

        println< ostream::HeadLevel::OK >( "Initialize the 8254 programmable interval timer." );
    }
    P8254::~P8254( VOID ) noexcept {
    }
}
