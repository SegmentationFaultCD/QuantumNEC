#include <kernel/interrupt/arch/x86_64/entry/entry_trap.hpp>
#include <kernel/print.hpp>
PUBLIC namespace QuantumNEC::Kernel::x86_64 {
    auto DivisionError::name( VOID ) noexcept -> VOID {
        std::println< print_level::ERROR >( "IRQ->0x00: #DE Divide-by-zero" );
    }
    auto DivisionError::error_code( [[maybe_unused]] uint64_t error_code ) noexcept -> VOID {
    }
    auto DivisionError::handler( Frame * frame ) noexcept -> Frame * {
        this->registers( frame );
        this->cpu( );
        this->task( );
        return frame;
    }

    auto Debug::name( VOID ) noexcept -> VOID {
        std::println< print_level::ERROR >( "IRQ->0x01: #DB Debug" );
    }
    auto Debug::error_code( [[maybe_unused]] uint64_t error_code ) noexcept -> VOID {
    }
    auto Debug::handler( Frame * frame ) noexcept -> Frame * {
        this->registers( frame );
        this->cpu( );
        this->task( );
        return frame;
    }

    auto NonMaskableInterrupt::name( VOID ) noexcept -> VOID {
        std::println< print_level::ERROR >( "IRQ->0x02: --- Non Maskable Interrupt(NMI)" );
    }
    auto NonMaskableInterrupt::error_code( [[maybe_unused]] uint64_t error_code ) noexcept -> VOID {
    }
    auto NonMaskableInterrupt::handler( Frame * frame ) noexcept -> Frame * {
        this->registers( frame );
        this->cpu( );
        this->task( );
        return frame;
    }

    auto Breakpoint::name( VOID ) noexcept -> VOID {
        std::println< print_level::ERROR >( "IRQ->0x03: #BP Breakpoint" );
    }
    auto Breakpoint::error_code( [[maybe_unused]] uint64_t error_code ) noexcept -> VOID {
    }
    auto Breakpoint::handler( Frame * frame ) noexcept -> Frame * {
        this->registers( frame );
        this->cpu( );
        this->task( );
        return frame;
    }

    auto Overflow::name( VOID ) noexcept -> VOID {
        std::println< print_level::ERROR >( "IRQ->0x04: #OF Overflow" );
    }
    auto Overflow::error_code( [[maybe_unused]] uint64_t error_code ) noexcept -> VOID {
    }
    auto Overflow::handler( Frame * frame ) noexcept -> Frame * {
        this->registers( frame );
        this->cpu( );
        this->task( );
        return frame;
    }

    auto BoundRangeExceeded::name( VOID ) noexcept -> VOID {
        std::println< print_level::ERROR >( "IRQ->0x05: #BR Bound Range Exceeded" );
    }
    auto BoundRangeExceeded::error_code( [[maybe_unused]] uint64_t error_code ) noexcept -> VOID {
    }
    auto BoundRangeExceeded::handler( Frame * frame ) noexcept -> Frame * {
        this->registers( frame );
        this->cpu( );
        this->task( );
        return frame;
    }

    auto InvalidOpcode::name( VOID ) noexcept -> VOID {
        std::println< print_level::ERROR >( "IRQ->0x06: #UD Invalid Opcode" );
    }
    auto InvalidOpcode::error_code( [[maybe_unused]] uint64_t error_code ) noexcept -> VOID {
    }
    auto InvalidOpcode::handler( Frame * frame ) noexcept -> Frame * {
        this->registers( frame );
        this->cpu( );
        this->task( );
        return frame;
    }

    auto DeviceNotAvailable::name( VOID ) noexcept -> VOID {
        std::println< print_level::ERROR >( "IRQ->0x07: #NM DeviceNotAvailable" );
    }
    auto DeviceNotAvailable::error_code( [[maybe_unused]] uint64_t error_code ) noexcept -> VOID {
    }
    auto DeviceNotAvailable::handler( Frame * frame ) noexcept -> Frame * {
        this->registers( frame );
        this->cpu( );
        this->task( );
        return frame;
    }

    auto DoubleFault::name( VOID ) noexcept -> VOID {
        std::println< print_level::ERROR >( "IRQ->0x08: #DF Double Fault" );
    }
    auto DoubleFault::error_code( [[maybe_unused]] uint64_t error_code ) noexcept -> VOID {
    }
    auto DoubleFault::handler( Frame * frame ) noexcept -> Frame * {
        this->registers( frame );
        this->cpu( );
        this->task( );
        return frame;
    }

    auto CoprocessorSegmentOverrun::name( VOID ) noexcept -> VOID {
        std::println< print_level::ERROR >( "IRQ->0x09: --- Coprocessor Segment Overrun" );
    }
    auto CoprocessorSegmentOverrun::error_code( [[maybe_unused]] uint64_t error_code ) noexcept -> VOID {
    }
    auto CoprocessorSegmentOverrun::handler( Frame * frame ) noexcept -> Frame * {
        this->registers( frame );
        this->cpu( );
        this->task( );
        return frame;
    }

    auto InvalidTSS::name( VOID ) noexcept -> VOID {
        std::println< print_level::ERROR >( "IRQ->0x0A: #TS Invalid TSS" );
    }

    auto InvalidTSS::handler( Frame * frame ) noexcept -> Frame * {
        this->registers( frame );
        this->cpu( );
        this->task( );
        return frame;
    }

    auto SegmentNotPresent::name( VOID ) noexcept -> VOID {
        std::println< print_level::ERROR >( "IRQ->0x0B: #NP Segment Not Present" );
    }

    auto SegmentNotPresent::handler( Frame * frame ) noexcept -> Frame * {
        this->registers( frame );
        this->cpu( );
        this->task( );
        return frame;
    }
    auto StackSegmentFault::name( VOID ) noexcept -> VOID {
        std::println< print_level::ERROR >( "IRQ->0x0C: #SS Stack Segment Fault" );
    }

    auto StackSegmentFault::handler( Frame * frame ) noexcept -> Frame * {
        this->registers( frame );
        this->cpu( );
        this->task( );
        return frame;
    }

    auto GeneralProtectionFault::name( VOID ) noexcept -> VOID {
        std::println< print_level::ERROR >( "IRQ->0x0D: #GP General Protection Fault" );
    }

    auto GeneralProtectionFault::handler( Frame * frame ) noexcept -> Frame * {
        this->registers( frame );
        this->cpu( );
        this->task( );
        return frame;
    }

    auto PageFault::name( VOID ) noexcept -> VOID {
        std::println< print_level::ERROR >( "IRQ->0x0E: #PF Page Fault" );
    }
    auto PageFault::error_code( uint64_t error_code ) noexcept -> VOID {
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
    auto PageFault::handler( Frame * frame ) noexcept -> Frame * {
        this->registers( frame );
        this->cpu( );
        this->task( );
        return frame;
    }

    auto X87FloatingPointException::name( VOID ) noexcept -> VOID {
        std::println< print_level::ERROR >( "IRQ->0x10: #MF x87 Floating-Point Exception" );
    }
    auto X87FloatingPointException::error_code( [[maybe_unused]] uint64_t error_code ) noexcept -> VOID {
    }
    auto X87FloatingPointException::handler( Frame * frame ) noexcept -> Frame * {
        this->registers( frame );
        this->cpu( );
        this->task( );
        return frame;
    }

    auto AlignmentCheck::name( VOID ) noexcept -> VOID {
        std::println< print_level::ERROR >( "IRQ->0x11: #AC Alignment Check" );
    }

    auto AlignmentCheck::handler( Frame * frame ) noexcept -> Frame * {
        this->registers( frame );
        this->cpu( );
        this->task( );
        return frame;
    }

    auto MachinCheck::name( VOID ) noexcept -> VOID {
        std::println< print_level::ERROR >( "IRQ->0x12: #MC Machine Check" );
    }
    auto MachinCheck::error_code( [[maybe_unused]] uint64_t error_code ) noexcept -> VOID {
    }
    auto MachinCheck::handler( Frame * frame ) noexcept -> Frame * {
        this->registers( frame );
        this->cpu( );
        this->task( );
        return frame;
    }

    auto SIMDFloatingPointException::name( VOID ) noexcept -> VOID {
        std::println< print_level::ERROR >( "IRQ->0x13: #XM/#XF SIMD Floating-Point Exception" );
    }
    auto SIMDFloatingPointException::error_code( [[maybe_unused]] uint64_t error_code ) noexcept -> VOID {
    }
    auto SIMDFloatingPointException::handler( Frame * frame ) noexcept -> Frame * {
        this->registers( frame );
        this->cpu( );
        this->task( );
        return frame;
    }

    auto VirtualizationException::name( VOID ) noexcept -> VOID {
        std::println< print_level::ERROR >( "IRQ->0x14: #VE Virtualization Exception" );
    }
    auto VirtualizationException::error_code( [[maybe_unused]] uint64_t error_code ) noexcept -> VOID {
    }
    auto VirtualizationException::handler( Frame * frame ) noexcept -> Frame * {
        this->registers( frame );
        this->cpu( );
        this->task( );
        return frame;
    }
    auto ControlProtectionException::name( VOID ) noexcept -> VOID {
        std::println< print_level::ERROR >( "IRQ->0x15: #CP Control Protection Exception" );
    }
    auto ControlProtectionException::error_code( [[maybe_unused]] uint64_t error_code ) noexcept -> VOID {
    }
    auto ControlProtectionException::handler( Frame * frame ) noexcept -> Frame * {
        this->registers( frame );
        this->cpu( );
        this->task( );
        return frame;
    }
    auto HypervisorInjectionException::name( VOID ) noexcept -> VOID {
        std::println< print_level::ERROR >( "IRQ->0x1C: #HV Hypervisor Injection Exception" );
    }
    auto HypervisorInjectionException::error_code( [[maybe_unused]] uint64_t error_code ) noexcept -> VOID {
    }
    auto HypervisorInjectionException::handler( Frame * frame ) noexcept -> Frame * {
        this->registers( frame );
        this->cpu( );
        this->task( );
        return frame;
    }
    auto VMMCommunicationException::name( VOID ) noexcept -> VOID {
        std::println< print_level::ERROR >( "IRQ->0x1D: #HV VMM Communication Exception" );
    }

    auto VMMCommunicationException::handler( Frame * frame ) noexcept -> Frame * {
        this->registers( frame );
        this->cpu( );
        this->task( );
        return frame;
    }
    auto SecurityException::name( VOID ) noexcept -> VOID {
        std::println< print_level::ERROR >( "IRQ->0x1E: #SX  Security Exception" );
    }

    auto SecurityException::handler( Frame * frame ) noexcept -> Frame * {
        this->registers( frame );
        this->cpu( );
        this->task( );
        return frame;
    }
}
