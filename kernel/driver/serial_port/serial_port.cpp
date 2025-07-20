#include <kernel/driver/cpu/io.hpp>
#include <kernel/driver/serial_port/serial_port.hpp>
using namespace Driver;

SerialPort::SerialPort( void ) noexcept {
}
auto SerialPort::initialize( void ) -> void { /*
    COM X +0	0	Read	Receive buffer.
    COM X +0	0	Write	Transmit buffer.
    COM X +1	0	Read/Write	Interrupt Enable Register.
    COM X +0	1	Read/Write	With DLAB set to 1, this is the least significant byte of the divisor value for setting the baud rate.
    COM X +1	1	Read/Write	With DLAB set to 1, this is the most significant byte of the divisor value.
    COM X +2	-	Read	Interrupt Identification
    COM X +2	-	Write	FIFO control registers
    COM X +3	-	Read/Write	Line Control Register. The most significant bit of this register is the DLAB.
    COM X +4	-	Read/Write	Modem Control Register.
    COM X +5	-	Read	Line Status Register.
    COM X +6	-	Read	Modem Status Register.
    COM X +7	-	Read/Write	Scratch Register.
 */

    IO::out8( COM1 + 1, 0x00 );     // Disable all interrupts
    IO::out8( COM1 + 3, 0x80 );     // Enable DLAB (set baud rate divisor)
    IO::out8( COM1 + 0, 0x03 );     // Set divisor to 3 (lo byte) 38400 baud
    IO::out8( COM1 + 1, 0x00 );     //                  (hi byte)
    IO::out8( COM1 + 3, 0x03 );     // 8 bits, no parity, one stop bit
    IO::out8( COM1 + 2, 0xC7 );     // Enable FIFO, clear them, with 14-byte threshold
    IO::out8( COM1 + 4, 0x0B );     // IRQs enabled, RTS/DSR set
    IO::out8( COM1 + 4, 0x1E );     // Set in loopback mode, test the serial chip
    IO::out8( COM1 + 0, 0xAE );     // Test serial chip (send byte 0xAE and check if serial returns same byte)

    // 检查串行是否有故障（即：与发送的字节不同）
    if ( IO::in8( COM1 + 0 ) != 0xAE ) {
        support_serial_port = false;
        // println< print_level::WARNING >( "Cannot enable serial port printing." );
    }
    else {
        IO::out8( COM1 + 4, 0x0F );
        // 如果串行没有故障，则将其设置为正常操作模式 （非环回，启用 IRQ 并启用 OUT#1 和 OUT#2 位）
        support_serial_port = true;
        // println< print_level::SYSTEM >( "serial port printing enable." );
    }
}
auto SerialPort::read( void ) -> char {
    if ( this->support_serial_port ) {
        while ( IO::in8( COM1 + 5 ) & 1 );
        return IO::in8( COM1 );
    }
    return '\0';
}
auto SerialPort::write( char ch ) -> void {
    if ( this->support_serial_port ) {
        while ( !( IO::in8( COM1 + 5 ) & 0x20 ) );
        IO::out8( COM1, ch );
    }
    return;
}