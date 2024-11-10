#include <kernel/global/arch/x86_64/global.hpp>
#include <kernel/interrupt/arch/x86_64/entry/entry.hpp>
#include <kernel/interrupt/arch/x86_64/entry/idt.hpp>
#include <kernel/interrupt/arch/x86_64/pic/pic.hpp>
#include <kernel/print.hpp>
using namespace QuantumNEC;
PUBLIC namespace QuantumNEC::Kernel::x86_64 {
    PRIVATE auto apic_error_handler _asmcall( IN CONST InterruptDescriptorTable::InterruptFrame * frame, IN uint64_t ) -> CONST InterruptDescriptorTable::InterruptFrame * {
        return frame;
    }
    ApicErrorEntry::ApicErrorEntry( VOID ) noexcept {
        InterruptDescriptorTable::InterruptFunctionController controller {
            .install   = nullptr,
            .uninstall = nullptr,
            .enable    = nullptr,
            .disable   = nullptr,
            .ack       = Apic::eoi
        };

        Apic::IOApicRedirectionEntry entry { };
        entry.vector         = IRQ_APIC_LVT_ERROR;
        entry.deliver_mode   = APIC_ICR_IOAPIC_FIXED;
        entry.dest_mode      = ICR_IOAPIC_DELV_PHYSICAL;
        entry.deliver_status = APIC_ICR_IOAPIC_IDLE;
        entry.polarity       = APIC_IOAPIC_POLARITY_HIGH;
        entry.irr            = APIC_IOAPIC_IRR_RESET;
        entry.trigger        = APIC_ICR_IOAPIC_EDGE;
        entry.mask           = APIC_ICR_IOAPIC_MASKED;

        InterruptDescriptorTable::register_irq( IRQ_APIC_LVT_ERROR, &entry, apic_error_handler, 0, "Local Apic LVT Error", &controller );
    }
}