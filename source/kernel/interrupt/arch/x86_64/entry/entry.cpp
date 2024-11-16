#include <kernel/interrupt/arch/x86_64/entry/entry.hpp>
#include <kernel/interrupt/arch/x86_64/entry/idt.hpp>
#include <kernel/interrupt/arch/x86_64/pic/apic.hpp>
#include <kernel/print.hpp>
using namespace QuantumNEC;
PUBLIC namespace QuantumNEC::Kernel::x86_64 {
    InterruptEntryRegister::InterruptEntryRegister( VOID ) noexcept :
        ClockEntry { }, SystemcallEntry { }, ApicErrorEntry { }, RTCEntry { }, Cascade_TimerEntry { }, SymmetricMultiprocessingSwitchCPUEntry { } {
    }
    InterruptEntryRegister::~InterruptEntryRegister( VOID ) noexcept {
    }

    PUBLIC _C_LINK _asmcall auto do_IRQ( IN CONST InterruptDescriptorTable::InterruptFrame * frame ) -> IN CONST InterruptDescriptorTable::InterruptFrame * {
        auto _frame { frame };

        if ( frame->vector <= 0x80 ) {
            auto &irq { InterruptDescriptorTable::interrupt_function_table[ frame->vector - IDT_ENTRY_IRQ_0 ] };
            if ( irq.handler && irq.controller.ack ) {
                irq.controller.ack( frame->vector );
                _frame = irq.handler( frame, irq.parameter );
            }
        }
        else {
            Apic::ioapic_edge_ack( frame->vector );
            auto &irq { InterruptDescriptorTable::SMP_IPI_function_table[ frame->vector - IDT_IRQ_SMP_INTERRUPT_0 ] };
            if ( irq.handler ) {
                _frame = irq.handler( frame, irq.parameter );
            }
        }
        return _frame;
    }
}
