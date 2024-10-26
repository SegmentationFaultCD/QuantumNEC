#include <kernel/interrupt/arch/x86_64/entry/entry.hpp>
#include <kernel/print.hpp>
#include <kernel/interrupt/arch/x86_64/entry/idt.hpp>
using namespace QuantumNEC;
PUBLIC namespace QuantumNEC::Kernel::x86_64 {
    InterruptEntryRegister::InterruptEntryRegister( VOID ) noexcept :
        ClockEntry { }, SystemcallEntry { }, ApicErrorEntry { }, RTCEntry { }, Cascade_TimerEntry { } {
    }
    InterruptEntryRegister::~InterruptEntryRegister( VOID ) noexcept {
    }

    PUBLIC _C_LINK _asmcall auto do_IRQ( IN CONST InterruptDescriptorTable::InterruptFrame * frame ) -> IN CONST InterruptDescriptorTable::InterruptFrame * {
        CONST InterruptDescriptorTable::InterruptFrame *_frame { frame };

        if ( frame->vector <= 0x80 ) {
            InterruptDescriptorTable::InterruptFunctionTable *irq { &InterruptDescriptorTable::interrupt_function_table[ frame->vector - IDT_ENTRY_IRQ_0 ] };
            if ( irq->handler && irq->controller.ack ) {
                _frame = irq->handler( frame, irq->parameter );
                irq->controller.ack( frame->vector );
            }
        }
        else {
            InterruptDescriptorTable::InterruptFunctionTable *irq { &InterruptDescriptorTable::interrupt_function_table[ frame->vector - IDT_ENTRY_IRQ_0 ] };
            if ( irq->handler && irq->controller.ack ) {
                _frame = irq->handler( frame, irq->parameter );
                irq->controller.ack( frame->vector );
            }
        }

        return _frame;
    }
}
