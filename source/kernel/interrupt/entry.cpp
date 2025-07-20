#include <kernel/driver/serial_port/serial_port.hpp>
extern "C" auto do_IRQ( ) -> void {
    Driver::SerialPort { }.write( 'h' );
    while ( true );
}