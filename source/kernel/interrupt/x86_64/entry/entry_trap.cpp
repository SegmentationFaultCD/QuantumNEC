#include <kernel/interrupt/x86_64/entry/entry_trap.hpp>
#include <kernel/print.hpp>
namespace QuantumNEC::Kernel::x86_64 {
using namespace std;
auto DivisionError::name( void ) noexcept -> void {
    std::println< print_level::ERROR >( "IRQ->0x00: #DE Divide-by-zero" );
}
auto DivisionError::error_code( [[maybe_unused]] uint64_t error_code ) noexcept -> void {
}
auto DivisionError::handler( Frame *frame ) noexcept -> Frame * {
    this->registers( frame );
    this->cpu( );
    this->task( );
    return frame;
}

auto Debug::name( void ) noexcept -> void {
    std::println< print_level::ERROR >( "IRQ->0x01: #DB Debug" );
}
auto Debug::error_code( [[maybe_unused]] uint64_t error_code ) noexcept -> void {
}
auto Debug::handler( Frame *frame ) noexcept -> Frame * {
    this->registers( frame );
    this->cpu( );
    this->task( );
    return frame;
}

auto NonMaskableInterrupt::name( void ) noexcept -> void {
    std::println< print_level::ERROR >( "IRQ->0x02: --- Non Maskable Interrupt(NMI)" );
}
auto NonMaskableInterrupt::error_code( [[maybe_unused]] uint64_t error_code ) noexcept -> void {
}
auto NonMaskableInterrupt::handler( Frame *frame ) noexcept -> Frame * {
    this->registers( frame );
    this->cpu( );
    this->task( );
    return frame;
}

auto Breakpoint::name( void ) noexcept -> void {
    std::println< print_level::ERROR >( "IRQ->0x03: #BP Breakpoint" );
}
auto Breakpoint::error_code( [[maybe_unused]] uint64_t error_code ) noexcept -> void {
}
auto Breakpoint::handler( Frame *frame ) noexcept -> Frame * {
    this->registers( frame );
    this->cpu( );
    this->task( );
    return frame;
}

auto Overflow::name( void ) noexcept -> void {
    std::println< print_level::ERROR >( "IRQ->0x04: #OF Overflow" );
}
auto Overflow::error_code( [[maybe_unused]] uint64_t error_code ) noexcept -> void {
}
auto Overflow::handler( Frame *frame ) noexcept -> Frame * {
    this->registers( frame );
    this->cpu( );
    this->task( );
    return frame;
}

auto BoundRangeExceeded::name( void ) noexcept -> void {
    std::println< print_level::ERROR >( "IRQ->0x05: #BR Bound Range Exceeded" );
}
auto BoundRangeExceeded::error_code( [[maybe_unused]] uint64_t error_code ) noexcept -> void {
}
auto BoundRangeExceeded::handler( Frame *frame ) noexcept -> Frame * {
    this->registers( frame );
    this->cpu( );
    this->task( );
    return frame;
}

auto InvalidOpcode::name( void ) noexcept -> void {
    std::println< print_level::ERROR >( "IRQ->0x06: #UD Invalid Opcode" );
}
auto InvalidOpcode::error_code( [[maybe_unused]] uint64_t error_code ) noexcept -> void {
}
auto InvalidOpcode::handler( Frame *frame ) noexcept -> Frame * {
    this->registers( frame );
    this->cpu( );
    this->task( );
    return frame;
}

auto DeviceNotAvailable::name( void ) noexcept -> void {
    std::println< print_level::ERROR >( "IRQ->0x07: #NM DeviceNotAvailable" );
}
auto DeviceNotAvailable::error_code( [[maybe_unused]] uint64_t error_code ) noexcept -> void {
}
auto DeviceNotAvailable::handler( Frame *frame ) noexcept -> Frame * {
    this->registers( frame );
    this->cpu( );
    this->task( );
    return frame;
}

auto DoubleFault::name( void ) noexcept -> void {
    std::println< print_level::ERROR >( "IRQ->0x08: #DF Double Fault" );
}
auto DoubleFault::error_code( [[maybe_unused]] uint64_t error_code ) noexcept -> void {
}
auto DoubleFault::handler( Frame *frame ) noexcept -> Frame * {
    this->registers( frame );
    this->cpu( );
    this->task( );
    return frame;
}

auto CoprocessorSegmentOverrun::name( void ) noexcept -> void {
    std::println< print_level::ERROR >( "IRQ->0x09: --- Coprocessor Segment Overrun" );
}
auto CoprocessorSegmentOverrun::error_code( [[maybe_unused]] uint64_t error_code ) noexcept -> void {
}
auto CoprocessorSegmentOverrun::handler( Frame *frame ) noexcept -> Frame * {
    this->registers( frame );
    this->cpu( );
    this->task( );
    return frame;
}

auto InvalidTSS::name( void ) noexcept -> void {
    std::println< print_level::ERROR >( "IRQ->0x0A: #TS Invalid TSS" );
}

auto InvalidTSS::handler( Frame *frame ) noexcept -> Frame * {
    this->registers( frame );
    this->cpu( );
    this->task( );
    return frame;
}

auto SegmentNotPresent::name( void ) noexcept -> void {
    std::println< print_level::ERROR >( "IRQ->0x0B: #NP Segment Not Present" );
}

auto SegmentNotPresent::handler( Frame *frame ) noexcept -> Frame * {
    this->registers( frame );
    this->cpu( );
    this->task( );
    return frame;
}
auto StackSegmentFault::name( void ) noexcept -> void {
    std::println< print_level::ERROR >( "IRQ->0x0C: #SS Stack Segment Fault" );
}

auto StackSegmentFault::handler( Frame *frame ) noexcept -> Frame * {
    this->registers( frame );
    this->cpu( );
    this->task( );
    return frame;
}

auto GeneralProtectionFault::name( void ) noexcept -> void {
    std::println< print_level::ERROR >( "IRQ->0x0D: #GP General Protection Fault" );
}

auto GeneralProtectionFault::handler( Frame *frame ) noexcept -> Frame * {
    this->registers( frame );
    this->cpu( );
    this->task( );
    return frame;
}

auto PageFault::name( void ) noexcept -> void {
    std::println< print_level::ERROR >( "IRQ->0x0E: #PF Page Fault" );
}
auto PageFault::error_code( uint64_t error_code ) noexcept -> void {
    PFErrorCode perror_code { error_code };
    if ( perror_code.present ) {
        std::println< print_level::ERROR >( "Page fault was caused by a page-protection violation" );
    }
    else {
        std::println< print_level::ERROR >( "Page fault was caused by a non-present page" );
    }

    if ( perror_code.write ) {
        std::println< print_level::ERROR >( "Page fault was caused by a write access." );
    }
    else {
        std::println< print_level::ERROR >( "Page fault was caused by a read access." );
    }

    if ( perror_code.user ) {
        std::println< print_level::ERROR >( "Page fault was in user mode(3)." );
    }
    else {
        std::println< print_level::ERROR >( "Page fault was in kernel mode(0,1,2)." );
    }

    if ( perror_code.reserved_write ) {
        std::println< print_level::ERROR >( "Page fault was caused by reading/writing reserved position." );
    }
    if ( perror_code.instruction_fetch ) {
        std::println< print_level::ERROR >( "Page fault was caused by an instruction fetch." );
    }
    if ( perror_code.protection_key ) {
        std::println< print_level::ERROR >( "Page fault was caused by a protection-key violation." );
    }
    if ( perror_code.shadow_stack ) {
        std::println< print_level::ERROR >( "Page fault was caused by a shadow stack access." );
    }
    if ( perror_code.software_guard_extensions ) {
        std::println< print_level::ERROR >( "Page fault was due to an SGX violation" );
    }
}
auto PageFault::handler( Frame *frame ) noexcept -> Frame * {
    this->registers( frame );
    this->cpu( );
    this->task( );
    return frame;
}

auto X87FloatingPointException::name( void ) noexcept -> void {
    std::println< print_level::ERROR >( "IRQ->0x10: #MF x87 Floating-Point Exception" );
}
auto X87FloatingPointException::error_code( [[maybe_unused]] uint64_t error_code ) noexcept -> void {
}
auto X87FloatingPointException::handler( Frame *frame ) noexcept -> Frame * {
    this->registers( frame );
    this->cpu( );
    this->task( );
    return frame;
}

auto AlignmentCheck::name( void ) noexcept -> void {
    std::println< print_level::ERROR >( "IRQ->0x11: #AC Alignment Check" );
}

auto AlignmentCheck::handler( Frame *frame ) noexcept -> Frame * {
    this->registers( frame );
    this->cpu( );
    this->task( );
    return frame;
}

auto MachinCheck::name( void ) noexcept -> void {
    std::println< print_level::ERROR >( "IRQ->0x12: #MC Machine Check" );
}
auto MachinCheck::error_code( [[maybe_unused]] uint64_t error_code ) noexcept -> void {
}
auto MachinCheck::handler( Frame *frame ) noexcept -> Frame * {
    this->registers( frame );
    this->cpu( );
    this->task( );
    return frame;
}

auto SIMDFloatingPointException::name( void ) noexcept -> void {
    std::println< print_level::ERROR >( "IRQ->0x13: #XM/#XF SIMD Floating-Point Exception" );
}
auto SIMDFloatingPointException::error_code( [[maybe_unused]] uint64_t error_code ) noexcept -> void {
}
auto SIMDFloatingPointException::handler( Frame *frame ) noexcept -> Frame * {
    this->registers( frame );
    this->cpu( );
    this->task( );
    return frame;
}

auto VirtualizationException::name( void ) noexcept -> void {
    std::println< print_level::ERROR >( "IRQ->0x14: #VE Virtualization Exception" );
}
auto VirtualizationException::error_code( [[maybe_unused]] uint64_t error_code ) noexcept -> void {
}
auto VirtualizationException::handler( Frame *frame ) noexcept -> Frame * {
    this->registers( frame );
    this->cpu( );
    this->task( );
    return frame;
}
auto ControlProtectionException::name( void ) noexcept -> void {
    std::println< print_level::ERROR >( "IRQ->0x15: #CP Control Protection Exception" );
}
auto ControlProtectionException::error_code( [[maybe_unused]] uint64_t error_code ) noexcept -> void {
}
auto ControlProtectionException::handler( Frame *frame ) noexcept -> Frame * {
    this->registers( frame );
    this->cpu( );
    this->task( );
    return frame;
}
auto HypervisorInjectionException::name( void ) noexcept -> void {
    std::println< print_level::ERROR >( "IRQ->0x1C: #HV Hypervisor Injection Exception" );
}
auto HypervisorInjectionException::error_code( [[maybe_unused]] uint64_t error_code ) noexcept -> void {
}
auto HypervisorInjectionException::handler( Frame *frame ) noexcept -> Frame * {
    this->registers( frame );
    this->cpu( );
    this->task( );
    return frame;
}
auto VMMCommunicationException::name( void ) noexcept -> void {
    std::println< print_level::ERROR >( "IRQ->0x1D: #HV VMM Communication Exception" );
}

auto VMMCommunicationException::handler( Frame *frame ) noexcept -> Frame * {
    this->registers( frame );
    this->cpu( );
    this->task( );
    return frame;
}
auto SecurityException::name( void ) noexcept -> void {
    std::println< print_level::ERROR >( "IRQ->0x1E: #SX  Security Exception" );
}

auto SecurityException::handler( Frame *frame ) noexcept -> Frame * {
    this->registers( frame );
    this->cpu( );
    this->task( );
    return frame;
}
}     // namespace QuantumNEC::Kernel::x86_64
