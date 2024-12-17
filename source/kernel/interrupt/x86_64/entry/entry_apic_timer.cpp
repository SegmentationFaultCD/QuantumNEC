#include <kernel/driver/output/x86_64/serial_port.hpp>
#include <kernel/global/x86_64/global.hpp>
#include <kernel/interrupt/x86_64/entry/entry_apic_timer.hpp>
#include <kernel/interrupt/x86_64/entry/idt.hpp>
#include <kernel/interrupt/x86_64/pic/pic.hpp>
#include <kernel/print.hpp>
using namespace QuantumNEC;
namespace QuantumNEC::Kernel::x86_64 {
auto ApicTimerEntry::name( void ) noexcept -> void {
}
auto ApicTimerEntry::error_code( [[maybe_unused]] uint64_t error_code ) noexcept -> void {
}
auto ApicTimerEntry::handler( Frame *frame ) noexcept -> Frame * {
    Apic::eoi( frame->vector );
    return frame;
}
auto ApicTimerEntry::do_register( void ) -> void {
    Apic::IOApicRedirectionEntry entry { };
    // entry.vector         = IRQ_APIC_LVT_TIMER;
    // entry.deliver_mode   = APIC_ICR_IOAPIC_FIXED;
    // entry.dest_mode      = ICR_IOAPIC_DELV_PHYSICAL;
    // entry.deliver_status = APIC_ICR_IOAPIC_IDLE;
    // entry.polarity       = APIC_IOAPIC_POLARITY_HIGH;
    // entry.irr            = APIC_IOAPIC_IRR_RESET;
    // entry.trigger        = APIC_ICR_IOAPIC_EDGE;
    // entry.mask           = APIC_ICR_IOAPIC_MASKED;
    // Apic::install_ioapic( IRQ_APIC_LVT_TIMER, &entry );
    // Apic::enable_ioapic( IRQ_APIC_LVT_TIMER );
}
}     // namespace QuantumNEC::Kernel::x86_64