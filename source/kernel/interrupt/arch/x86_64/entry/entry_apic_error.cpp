#include <kernel/global/arch/x86_64/global.hpp>
#include <kernel/interrupt/arch/x86_64/entry/entry_apic_error.hpp>
#include <kernel/interrupt/arch/x86_64/entry/idt.hpp>
#include <kernel/interrupt/arch/x86_64/pic/pic.hpp>
#include <kernel/print.hpp>
using namespace QuantumNEC;
PUBLIC namespace QuantumNEC::Kernel::x86_64 {
    auto ApicErrorEntry::name( VOID ) noexcept -> VOID {
    }
    auto ApicErrorEntry::error_code( [[maybe_unused]] uint64_t error_code ) noexcept -> VOID {
    }
    auto ApicErrorEntry::handler( Frame * frame ) noexcept -> Frame * {
        return frame;
    }
    auto ApicErrorEntry::do_register( VOID ) -> VOID {
        Apic::IOApicRedirectionEntry entry { };
        entry.vector         = IRQ_APIC_LVT_ERROR;
        entry.deliver_mode   = APIC_ICR_IOAPIC_FIXED;
        entry.dest_mode      = ICR_IOAPIC_DELV_PHYSICAL;
        entry.deliver_status = APIC_ICR_IOAPIC_IDLE;
        entry.polarity       = APIC_IOAPIC_POLARITY_HIGH;
        entry.irr            = APIC_IOAPIC_IRR_RESET;
        entry.trigger        = APIC_ICR_IOAPIC_EDGE;
        entry.mask           = APIC_ICR_IOAPIC_MASKED;
        Apic::install_ioapic( IRQ_APIC_LVT_ERROR, &entry );
        Apic::enable_ioapic( IRQ_APIC_LVT_ERROR );
    }
}