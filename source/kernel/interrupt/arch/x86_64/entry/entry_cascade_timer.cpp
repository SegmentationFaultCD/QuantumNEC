#include <kernel/cpu/cpu.hpp>
#include <kernel/global/arch/x86_64/global.hpp>
#include <kernel/interrupt/arch/x86_64/entry/entry.hpp>
#include <kernel/interrupt/arch/x86_64/entry/idt.hpp>
#include <kernel/interrupt/arch/x86_64/pic/pic.hpp>
#include <kernel/print.hpp>
using namespace QuantumNEC;
PUBLIC namespace QuantumNEC::Kernel::x86_64 {
    _C_LINK auto save_current_frame( IN CONST InterruptDescriptorTable::InterruptFrame * frame ) -> VOID;

    PRIVATE auto cascade_timer_handler _asmcall( IN CONST InterruptDescriptorTable::InterruptFrame * frame, IN uint64_t )
        -> CONST                       InterruptDescriptorTable::InterruptFrame                       *{
        Cascade_TimerEntry::global_jiffies++;
        CPU::switch_cpu(  );

        return frame;
    }
    Cascade_TimerEntry::Cascade_TimerEntry( VOID ) noexcept {
        InterruptDescriptorTable::InterruptFunctionController controller {
            .install   = Apic::install_ioapic,
            .uninstall = Apic::uninstall_ioapic,
            .enable    = Apic::enable_ioapic,
            .disable   = Apic::disable_ioapic,
            .ack       = Apic::eoi
        };

        Apic::IOApicRedirectionEntry entry { };
        entry.vector         = IRQ_CASCADE_TIMER;
        entry.deliver_mode   = APIC_ICR_IOAPIC_FIXED;
        entry.dest_mode      = ICR_IOAPIC_DELV_PHYSICAL;
        entry.deliver_status = APIC_ICR_IOAPIC_IDLE;
        entry.polarity       = APIC_IOAPIC_POLARITY_HIGH;
        entry.irr            = APIC_IOAPIC_IRR_RESET;
        entry.trigger        = APIC_ICR_IOAPIC_EDGE;
        entry.mask           = APIC_ICR_IOAPIC_MASKED;

        InterruptDescriptorTable::register_irq( IRQ_CASCADE_TIMER, &entry, cascade_timer_handler, 0, "CASCADE / HPET Timer 0", &controller );
    }
}