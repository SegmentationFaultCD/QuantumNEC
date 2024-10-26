#include <kernel/interrupt/arch/x86_64/entry/entry.hpp>
#include <kernel/print.hpp>
#include <kernel/global/arch/x86_64/global.hpp>
#include <kernel/interrupt/arch/x86_64/entry/idt.hpp>
#include <kernel/interrupt/arch/x86_64/pic/pic.hpp>
using namespace QuantumNEC;
PUBLIC namespace QuantumNEC::Kernel::x86_64 {
    PRIVATE auto _asmcall rtc_handler( IN CONST InterruptDescriptorTable::InterruptFrame * frame, IN uint64_t ) -> CONST InterruptDescriptorTable::InterruptFrame * {
        return frame;
    }

    RTCEntry::RTCEntry( VOID ) noexcept {
        InterruptDescriptorTable::InterruptFunctionController controller {
            .install = Apic::install_ioapic,
            .uninstall = Apic::uninstall_ioapic,
            .enable = Apic::enable_ioapic,
            .disable = Apic::disable_ioapic,
            .ack = Apic::eoi
        };

        Apic::IOApicRedirectionEntry entry { };
        entry.vector = IRQ_CMOS_RTC;
        entry.deliver_mode = APIC_ICR_IOAPIC_FIXED;
        entry.dest_mode = ICR_IOAPIC_DELV_PHYSICAL;
        entry.deliver_status = APIC_ICR_IOAPIC_IDLE;
        entry.polarity = APIC_IOAPIC_POLARITY_HIGH;
        entry.irr = APIC_IOAPIC_IRR_RESET;
        entry.trigger = APIC_ICR_IOAPIC_EDGE;
        entry.mask = APIC_ICR_IOAPIC_MASKED;

        InterruptDescriptorTable::register_irq( IRQ_CMOS_RTC, &entry, rtc_handler, 0, "CMOS RTC", &controller );
    }
}