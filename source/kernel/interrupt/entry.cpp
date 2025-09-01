#include <kernel/display/print.hpp>
#include <kernel/driver/cpu/io.hpp>
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
using namespace Display;
auto GeneralInterruptHandle::name( std::uint64_t vector ) noexcept -> void {
    Display::println( "{}", this->error_name[ vector ].data( ) );
}
auto GeneralInterruptHandle::registers( IDT::Frame *frame ) noexcept -> void {
    Display::println( "RIP:{}", frame->rip );
    Display::println( "RSP:{:x} SS:{:x} CS:{:x}\n"
                      "Rlfags:{} DS:{:x} ES:{:x} FS:{:x} GS:{:x}\n"
                      "R8:{:x} R9:{:x} R10:{:x} R11:{:x} R12:{:x} R13:{:x} R14:{:x} R15:{:x}\n"
                      "RAX:{:x} RBX:{:x} RCX:{:x} RDX:{:x}\n"
                      "RBP:{:x} RSI:{:x} RDI:{:x}\n"
                      "CR0:{:x} CR2:{:x} CR3:{:x} CR4:{:x} CR8:{:x}",
                      frame->rsp, frame->ss, frame->cs, (uint64_t)frame->rflags, frame->regs.ds, frame->regs.es, frame->regs.fs, frame->regs.gs,
                      frame->regs.r8, frame->regs.r9, frame->regs.r10, frame->regs.r11, frame->regs.r12, frame->regs.r13, frame->regs.r14, frame->regs.r15,
                      frame->regs.rax, frame->regs.rbx, frame->regs.rcx, frame->regs.rdx,
                      frame->regs.rbp, frame->regs.rsi, frame->regs.rdi,
                      (uint64_t)Driver::IO::read_cr0( ),
                      (uint64_t)Driver::IO::read_cr2( ),
                      (uint64_t)Driver::IO::read_cr3( ),
                      (uint64_t)Driver::IO::read_cr4( ),
                      (uint64_t)Driver::IO::read_cr8( ) );
}
auto GeneralInterruptHandle::error_code( std::uint64_t code ) noexcept -> void {
}
auto GeneralInterruptHandle::task( ) noexcept -> void {
}
auto GeneralInterruptHandle::cpu( ) noexcept -> void {
    // Display::println("{}", Apic::);
}
auto GeneralInterruptHandle::handler( IDT::Frame *frame ) noexcept -> IDT::Frame * {
    while ( true );
    return frame;
}

auto PageFault::error_code( std::uint64_t code ) noexcept -> void {
    ErrorCode c_code { code };
    using namespace Display;
    Library::cxxstring str;
    if ( c_code.present ) {
        str += "Page fault was caused by a page-protection violation, ";
    }
    else {
        str += "Page fault was caused by a non-present page, ";
    }

    if ( c_code.write ) {
        str += "Page fault was caused by a write access. ";
    }
    else {
        str += "Page fault was caused by a read access. ";
    }

    if ( c_code.user ) {
        str += "Page fault was in user mode(3). ";
    }
    else {
        str += "Page fault was in kernel mode(0,1,2). ";
    }

    if ( c_code.reserved_write ) {
        str += "Page fault was caused by reading/writing reserved position. ";
    }
    if ( c_code.instruction_fetch ) {
        str += "Page fault was caused by an instruction fetch.\n";
    }

    if ( c_code.protection_key ) {
        str += "Page fault was caused by a protection-key violation. \n";
    }
    if ( c_code.shadow_stack ) {
        str += "Page fault was caused by a shadow stack access. \n";
    }
    if ( c_code.software_guard_extensions ) {
        str += "Page fault was due to an SGX violation. \n";
    }
    if ( c_code.RMP ) {
        str += "Page fault was caused by an RMP violation.\n";
    }
    else {
    }
    println( "{}", str.c_str( ) );
}
auto InvalidTSS::error_code( std::uint64_t code ) noexcept -> void {
    ErrorCode c_code { code };
    const char *tbl;
    using namespace Display;
    if ( c_code.external ) {
        println( "The exception originated externally to the processor." );
    }
    switch ( c_code.tbl ) {
    case 0b00:
        tbl = R"(GDT)";
        println( "The selector index references a descriptor in the GDT." );
        break;
    case 0b01:
    case 0b11:
        tbl = R"(IDT)";
        println( "The selector index references a descriptor in the IDT." );
        break;
    case 0b10:
        tbl = R"(LDT)";
        println( "The selector index references a descriptor in the LDT." );
        break;
    default:
        break;
    }
    println( "The selector index in the {} is {}.", tbl, (uint32_t)c_code.index );
}
auto SegmentNotPresent::error_code( std::uint64_t code ) noexcept -> void {
    ErrorCode c_code { code };
    const char *tbl;
    using namespace Display;
    if ( c_code.external ) {
        println( "The exception originated externally to the processor." );
    }
    switch ( c_code.tbl ) {
    case 0b00:
        tbl = R"(GDT)";
        println( "The selector index references a descriptor in the GDT." );
        break;
    case 0b01:
    case 0b11:
        tbl = R"(IDT)";
        println( "The selector index references a descriptor in the IDT." );
        break;
    case 0b10:
        tbl = R"(LDT)";
        println( "The selector index references a descriptor in the LDT." );
        break;
    default:
        break;
    }
    println( "The selector index in the {} is {}.", tbl, (uint32_t)c_code.index );
}
auto StackSegmentFault::error_code( std::uint64_t code ) noexcept -> void {
    ErrorCode c_code { code };
    const char *tbl;
    using namespace Display;
    if ( c_code.external ) {
        println( "The exception originated externally to the processor." );
    }
    switch ( c_code.tbl ) {
    case 0b00:
        tbl = R"(GDT)";
        println( "The selector index references a descriptor in the GDT." );
        break;
    case 0b01:
    case 0b11:
        tbl = R"(IDT)";
        println( "The selector index references a descriptor in the IDT." );
        break;
    case 0b10:
        tbl = R"(LDT)";
        println( "The selector index references a descriptor in the LDT." );
        break;
    default:
        break;
    }
    println( "The selector index in the {} is {}.", tbl, (uint32_t)c_code.index );
}
auto GeneralProtectionFault::error_code( std::uint64_t code ) noexcept -> void {
    ErrorCode c_code { code };
    const char *tbl;
    using namespace Display;
    if ( c_code.external ) {
        println( "The exception originated externally to the processor." );
    }
    switch ( c_code.tbl ) {
    case 0b00:
        tbl = R"(GDT)";
        println( "The selector index references a descriptor in the GDT." );
        break;
    case 0b01:
    case 0b11:
        tbl = R"(IDT)";
        println( "The selector index references a descriptor in the IDT." );
        break;
    case 0b10:
        tbl = R"(LDT)";
        println( "The selector index references a descriptor in the LDT." );
        break;
    default:
        break;
    }
    println( "The selector index in the {} is {}.", tbl, (uint32_t)c_code.index );
}
auto ControlProtectionException::error_code( std::uint64_t code ) noexcept -> void {
    ErrorCode c_code { code };
    using namespace Driver;
    switch ( c_code ) {
    case 1:     // NERE-RET
        println( "A RET (near) instruction encountered a return mismatch." );
        break;
    case 2:     // FAR-RET / IREQ
        println( "A RET (far) or IRET instruction encountered a return mismatch." );
        break;
    case 3:     // RSTORSSP
        println( "An RSTORSSP instruction encountered an invalid shadow stack restore token." );
        break;
    case 4:     // SETSSBSY
        println( "An SETSSBSY instruction encountered an invalid supervisor shadow stack restore token." );
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