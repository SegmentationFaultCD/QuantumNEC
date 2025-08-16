#include <kernel/driver/cpu/io.hpp>
#include <kernel/driver/serial_port/serial_port.hpp>
#include <kernel/interrupt/apic.hpp>
#include <kernel/interrupt/entry.hpp>
#include <lib/string.hpp>
using namespace Interrupt;
extern "C" auto do_IRQ( IDT::Frame *frame ) -> IDT::Frame * {
    if ( auto entry = GeneralInterruptHandle::handlers[ frame->vector ]; entry ) {
        return entry->handle( frame );
    }
    else {
        while ( true ) __asm__ __volatile__( "hlt\n\t" );
    }
    return frame;
}

namespace Interrupt {
auto GeneralInterruptHandle::name( std::uint64_t vector ) noexcept -> void {
    Driver::SerialPort { }.print( this->error_name[ vector ].data( ) );
    Driver::SerialPort { }.print( "\n" );
}
auto GeneralInterruptHandle::registers( IDT::Frame *frame ) noexcept -> void {
    auto cr2 = Driver::IO::read_cr2( );
    char buf[ 114 ];
    Library::utoa( cr2.PFLA, buf, 16 );
    Driver::SerialPort { }.print( buf );
    Driver::SerialPort { }.print( "\n" );
    Library::utoa( (uint64_t)frame->rip, buf, 16 );
    Driver::SerialPort { }.print( buf );
    Driver::SerialPort { }.print( "\n" );
}
auto GeneralInterruptHandle::error_code( std::uint64_t code ) noexcept -> void {
}
auto GeneralInterruptHandle::task( ) noexcept -> void {
}
auto GeneralInterruptHandle::cpu( ) noexcept -> void {
}
auto GeneralInterruptHandle::handler( IDT::Frame *frame ) noexcept -> IDT::Frame * {
    while ( true );
    return frame;
}

auto PageFault::error_code( std::uint64_t code ) noexcept -> void {
    ErrorCode c_code { code };
    if ( c_code.present ) {
    }
    else {
    }
    if ( c_code.write ) {
    }
    else {
    }
    if ( c_code.user ) {
    }
    else {
    }
    if ( c_code.reserved_write ) {
    }
    if ( c_code.instruction_fetch ) {
    }
    if ( c_code.protection_key ) {
    }
    if ( c_code.shadow_stack ) {
    }
    if ( c_code.software_guard_extensions ) {
    }
    if ( c_code.RMP ) {
    }
    else {
    }
}
auto InvalidTSS::error_code( std::uint64_t code ) noexcept -> void {
    ErrorCode c_code { code };
    if ( c_code.external ) {
    }
    const char *tbl;
    switch ( c_code.tbl ) {
    case 0b00:
        tbl = R"(GDT)";
        break;
    case 0b01:
    case 0b11:
        tbl = R"(IDT)";
        break;
    case 0b10:
        tbl = R"(LDT)";
        break;
    default:
        break;
    }
}
auto SegmentNotPresent::error_code( std::uint64_t code ) noexcept -> void {
    ErrorCode c_code { code };
    if ( c_code.external ) {
    }
    const char *tbl;
    switch ( c_code.tbl ) {
    case 0b00:
        tbl = R"(GDT)";
        break;
    case 0b01:
    case 0b11:
        tbl = R"(IDT)";
        break;
    case 0b10:
        tbl = R"(LDT)";
        break;
    default:
        break;
    }
}
auto StackSegmentFault::error_code( std::uint64_t code ) noexcept -> void {
    ErrorCode c_code { code };
    if ( c_code.external ) {
    }
    const char *tbl;
    switch ( c_code.tbl ) {
    case 0b00:
        tbl = R"(GDT)";
        break;
    case 0b01:
    case 0b11:
        tbl = R"(IDT)";
        break;
    case 0b10:
        tbl = R"(LDT)";
        break;
    default:
        break;
    }
}
auto GeneralProtectionFault::error_code( std::uint64_t code ) noexcept -> void {
    ErrorCode c_code { code };
    if ( c_code.external ) {
    }
    const char *tbl;
    switch ( c_code.tbl ) {
    case 0b00:
        tbl = R"(GDT)";
        break;
    case 0b01:
    case 0b11:
        tbl = R"(IDT)";
        break;
    case 0b10:
        tbl = R"(LDT)";
        break;
    default:
        break;
    }
}
auto ControlProtectionException::error_code( std::uint64_t code ) noexcept -> void {
    ErrorCode c_code { code };
    switch ( c_code ) {
    case 1:     // NERE-RET

        break;
    case 2:     // FAR-RET / IREQ

        break;
    case 3:     // RSTORSSP

        break;
    case 4:     // SETSSBSY

        break;
    }
}
auto HypervisorInjectionException::error_code( std::uint64_t code ) noexcept -> void {
}
auto VMMCommunicationException::error_code( std::uint64_t code ) noexcept -> void {
}
auto SecurityException::error_code( std::uint64_t code ) noexcept -> void {
}
}     // namespace Interrupt