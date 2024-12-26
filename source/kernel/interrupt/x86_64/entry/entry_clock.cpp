#include <kernel/global/x86_64/global.hpp>
#include <kernel/interrupt/x86_64/entry/entry_clock.hpp>
#include <kernel/interrupt/x86_64/entry/idt.hpp>
#include <kernel/interrupt/x86_64/pic/pic.hpp>

using namespace QuantumNEC;
namespace QuantumNEC::Kernel::x86_64 {

auto ClockEntry::name( void ) noexcept -> void {
}
auto ClockEntry::error_code( [[maybe_unused]] uint64_t error_code ) noexcept -> void {
}
auto ClockEntry::handler( Frame *frame ) noexcept -> Frame * {
    return frame;
}
auto ClockEntry::do_register( void ) -> void {
    Apic::IOApicRedirectionEntry entry { };
    entry.vector         = IRQ_CLOCK;
    entry.deliver_mode   = APIC_ICR_IOAPIC_FIXED;
    entry.dest_mode      = ICR_IOAPIC_DELV_PHYSICAL;
    entry.deliver_status = APIC_ICR_IOAPIC_IDLE;
    entry.polarity       = APIC_IOAPIC_POLARITY_HIGH;
    entry.irr            = APIC_IOAPIC_IRR_RESET;
    entry.trigger        = APIC_ICR_IOAPIC_EDGE;
    entry.mask           = APIC_ICR_IOAPIC_MASKED;
    Apic::install_ioapic( IRQ_CLOCK, &entry );
}
}     // namespace QuantumNEC::Kernel::x86_64