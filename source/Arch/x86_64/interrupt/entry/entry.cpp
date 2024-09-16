#include <Arch/x86_64/platform/platform.hpp>

PUBLIC namespace QuantumNEC::Architecture {
    InterruptEntryRegister::InterruptEntryRegister( VOID ) noexcept :
        ClockEntry { }, SystemcallEntry { }, ApicErrorEntry { }, RTCEntry { }, Cascade_TimerEntry { } {
    }
    InterruptEntryRegister::~InterruptEntryRegister( VOID ) noexcept {
    }

    PUBLIC _C_LINK ASMCALL auto do_IRQ( IN CONST InterruptDescriptorTable::InterruptFrame * frame )->IN CONST InterruptDescriptorTable::InterruptFrame * {
        CONST InterruptDescriptorTable::InterruptFrame *_frame { frame };

        if ( frame->vector <= 0x80 ) {
            InterruptDescriptorTable::InterruptFunctionTable *irq { &InterruptDescriptorTable::interrupt_function_table[ frame->vector - IDT_ENTRY_IRQ_0 ] };
            if ( irq->handler && irq->controller.ack ) {
                _frame = irq->handler( frame, irq->parameter );
                irq->controller.ack( frame->vector );
            }
        }
        else {
            Apic::InterruptCommandRegister icr { };
            icr.deliver_mode = ICR_START_UP;
            icr.dest_mode = ICR_IOAPIC_DELV_PHYSICAL;
            icr.deliver_status = APIC_ICR_IOAPIC_IDLE;
            icr.level = ICR_LEVEL_DE_ASSERT;
            icr.trigger = APIC_ICR_IOAPIC_EDGE;
            icr.dest_shorthand = ICR_ALL_EXCLUDE_SELF;
            icr.destination.x2apic_destination = 0x00;
            CPUs::wrmsr( LOCAL_APIC_MSR_ICRLO, icr );
        }

        return _frame;
    }
}