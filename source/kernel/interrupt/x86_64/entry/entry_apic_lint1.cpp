#include <kernel/global/x86_64/global.hpp>
#include <kernel/interrupt/x86_64/entry/entry_apic_lint1.hpp>
#include <kernel/interrupt/x86_64/entry/idt.hpp>
#include <kernel/interrupt/x86_64/pic/pic.hpp>
#include <kernel/print.hpp>
using namespace QuantumNEC;
namespace QuantumNEC::Kernel::x86_64 {
auto ApicLINT1Entry::name( void ) noexcept -> void {
}
auto ApicLINT1Entry::error_code( [[maybe_unused]] uint64_t error_code ) noexcept -> void {
}
auto ApicLINT1Entry::handler( Frame *frame ) noexcept -> Frame * {
    return frame;
}
auto ApicLINT1Entry::do_register( void ) -> void {
    Apic::IOApicRedirectionEntry entry { };
    entry.vector         = IRQ_APIC_LVT_LINT1;
    entry.deliver_mode   = APIC_ICR_IOAPIC_FIXED;
    entry.dest_mode      = ICR_IOAPIC_DELV_PHYSICAL;
    entry.deliver_status = APIC_ICR_IOAPIC_IDLE;
    entry.polarity       = APIC_IOAPIC_POLARITY_HIGH;
    entry.irr            = APIC_IOAPIC_IRR_RESET;
    entry.trigger        = APIC_ICR_IOAPIC_EDGE;
    entry.mask           = APIC_ICR_IOAPIC_MASKED;
    Apic::install_ioapic( IRQ_APIC_LVT_LINT1, &entry );
    Apic::enable_ioapic( IRQ_APIC_LVT_LINT1 );
}
}     // namespace QuantumNEC::Kernel::x86_64