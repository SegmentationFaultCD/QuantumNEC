#include <Arch/x86_64/platform/platform.hpp>
#include <Lib/spin_lock.hpp>

using namespace QuantumNEC;
PUBLIC namespace QuantumNEC::Architecture {
    PRIVATE Lib::SpinLock lock { };
    _C_LINK auto save_current_frame( IN CONST InterruptDescriptorTable::InterruptFrame * frame )->VOID;
    PRIVATE auto ASMCALL clock_handler( IN CONST InterruptDescriptorTable::InterruptFrame * frame, uint64_t )->CONST InterruptDescriptorTable::InterruptFrame * {
        return frame;
    }
    ClockEntry::ClockEntry( VOID ) noexcept {
        InterruptDescriptorTable::InterruptFunctionController controller {
            .install = NULL,
            .uninstall = NULL,
            .enable = NULL,
            .disable = NULL,
            .ack = PIC8259A::eoi
        };
        Architecture::InterruptDescriptorTable::register_irq( IRQ_CLOCK, NULL, clock_handler, 0, "8259A PIC clock interrupt", &controller );
    }
}