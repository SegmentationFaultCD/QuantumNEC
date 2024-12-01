#include <kernel/cpu/cpu.hpp>
#include <kernel/driver/output/arch/x86_64/serial_port.hpp>
#include <kernel/print.hpp>
PUBLIC namespace QuantumNEC::Kernel::x86_64 {
    using namespace std;
    SerialPort::SerialPort( VOID ) noexcept {
        /*
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

        CPU::io_out8( COM1 + 1, 0x00 );     // Disable all interrupts
        CPU::io_out8( COM1 + 3, 0x80 );     // Enable DLAB (set baud rate divisor)
        CPU::io_out8( COM1 + 0, 0x03 );     // Set divisor to 3 (lo byte) 38400 baud
        CPU::io_out8( COM1 + 1, 0x00 );     //                  (hi byte)
        CPU::io_out8( COM1 + 3, 0x03 );     // 8 bits, no parity, one stop bit
        CPU::io_out8( COM1 + 2, 0xC7 );     // Enable FIFO, clear them, with 14-byte threshold
        CPU::io_out8( COM1 + 4, 0x0B );     // IRQs enabled, RTS/DSR set
        CPU::io_out8( COM1 + 4, 0x1E );     // Set in loopback mode, test the serial chip
        CPU::io_out8( COM1 + 0, 0xAE );     // Test serial chip (send byte 0xAE and check if serial returns same byte)

        // 检查串行是否有故障（即：与发送的字节不同）
        if ( CPU::io_in8( COM1 + 0 ) != 0xAE ) {
            this->support_serial_port = FALSE;
            println< print_level::WARNING >( "Cannot enable serial port printing." );
        }
        else {
            CPU::io_out8( COM1 + 4, 0x0F );
            // 如果串行没有故障，则将其设置为正常操作模式 （非环回，启用 IRQ 并启用 OUT#1 和 OUT#2 位）
            this->support_serial_port = TRUE;
            println< print_level::SYSTEM >( "serial port printing enable." );
        }
    }
    auto SerialPort::read( VOID ) -> char_t {
        if ( support_serial_port ) {
            while ( CPU::io_in8( COM1 + 5 ) & 1 );
            return CPU::io_in8( COM1 );
        }
        return '\0';
    }
    auto SerialPort::write( IN char_t ch ) -> VOID {
        if ( support_serial_port ) {
            while ( !( CPU::io_in8( COM1 + 5 ) & 0x20 ) );
            CPU::io_out8( COM1, ch );
        }
        return;
    }
}