#include <kernel/global/arch/x86_64/global.hpp>
#include <kernel/interrupt/arch/x86_64/entry/entry_rtc.hpp>
#include <kernel/interrupt/arch/x86_64/entry/idt.hpp>
#include <kernel/interrupt/arch/x86_64/pic/pic.hpp>
#include <kernel/print.hpp>
using namespace QuantumNEC;
PUBLIC namespace QuantumNEC::Kernel::x86_64 {
    auto RTCEntry::name( VOID ) noexcept -> VOID {
    }
    auto RTCEntry::error_code( [[maybe_unused]] uint64_t error_code ) noexcept -> VOID {
    }
    auto RTCEntry::handler( Frame * frame ) noexcept -> Frame * {
        return frame;
    }
    auto RTCEntry::do_register( VOID ) -> VOID {
        Apic::IOApicRedirectionEntry entry { };
        entry.vector         = IRQ_CMOS_RTC;
        entry.deliver_mode   = APIC_ICR_IOAPIC_FIXED;
        entry.dest_mode      = ICR_IOAPIC_DELV_PHYSICAL;
        entry.deliver_status = APIC_ICR_IOAPIC_IDLE;
        entry.polarity       = APIC_IOAPIC_POLARITY_HIGH;
        entry.irr            = APIC_IOAPIC_IRR_RESET;
        entry.trigger        = APIC_ICR_IOAPIC_EDGE;
        entry.mask           = APIC_ICR_IOAPIC_MASKED;
        Apic::install_ioapic( IRQ_CMOS_RTC, &entry );
    }
}