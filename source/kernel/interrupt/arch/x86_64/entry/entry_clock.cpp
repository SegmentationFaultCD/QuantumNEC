#include <kernel/global/arch/x86_64/global.hpp>
#include <kernel/interrupt/arch/x86_64/entry/entry.hpp>
#include <kernel/interrupt/arch/x86_64/entry/idt.hpp>
#include <kernel/interrupt/arch/x86_64/pic/pic.hpp>
#include <kernel/print.hpp>
using namespace QuantumNEC;
PUBLIC namespace QuantumNEC::Kernel::x86_64 {
    PRIVATE Lib::SpinLock lock { };

    _C_LINK auto save_current_frame( IN CONST InterruptDescriptorTable::InterruptFrame * frame ) -> VOID;

    PRIVATE auto clock_handler _asmcall( IN CONST InterruptDescriptorTable::InterruptFrame * frame, uint64_t ) -> CONST InterruptDescriptorTable::InterruptFrame * {
        return frame;
    }
    ClockEntry::ClockEntry( VOID ) noexcept {
        InterruptDescriptorTable::InterruptFunctionController controller {
            .install   = NULL,
            .uninstall = NULL,
            .enable    = NULL,
            .disable   = NULL,
            .ack       = PIC8259A::eoi
        };
        InterruptDescriptorTable::register_irq( IRQ_CLOCK, NULL, clock_handler, 0, "8259A PIC clock interrupt", &controller );
    }
}