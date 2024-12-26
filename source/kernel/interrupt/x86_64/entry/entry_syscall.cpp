#include <kernel/cpu/cpu.hpp>
#include <kernel/global/x86_64/global.hpp>
#include <kernel/interrupt/x86_64/entry/entry_syscall.hpp>
#include <kernel/interrupt/x86_64/entry/idt.hpp>
#include <kernel/interrupt/x86_64/pic/pic.hpp>
#include <kernel/print.hpp>
#include <kernel/syscall/syscall.hpp>
using namespace QuantumNEC;
namespace QuantumNEC::Kernel::x86_64 {

auto SystemcallEntry::name( void ) noexcept -> void {
}
auto SystemcallEntry::error_code( [[maybe_unused]] uint64_t error_code ) noexcept -> void {
}
auto SystemcallEntry::handler( Frame *frame ) noexcept -> Frame * {
    Apic::eoi( frame->vector );
    // auto entry = Syscall::get_syscall_table( )[ frame->regs.rax ];
    // if ( !entry ) {
    //     frame->regs.rax = static_cast< uint64_t >( Syscall::Status::NO_SYSCALL );
    //     return frame;
    // }
    std::print( "S" );
    return frame;
    // return entry( frame );
}
auto SystemcallEntry::do_register( void ) -> void {
    Apic::IOApicRedirectionEntry entry { };
    entry.vector         = IRQ_SYSTEM_CALL;
    entry.deliver_mode   = APIC_ICR_IOAPIC_FIXED;
    entry.dest_mode      = ICR_IOAPIC_DELV_PHYSICAL;
    entry.deliver_status = APIC_ICR_IOAPIC_IDLE;
    entry.polarity       = APIC_IOAPIC_POLARITY_HIGH;
    entry.irr            = APIC_IOAPIC_IRR_RESET;
    entry.trigger        = APIC_ICR_IOAPIC_EDGE;
    entry.mask           = APIC_ICR_IOAPIC_MASKED;
    Apic::install_ioapic( IRQ_SYSTEM_CALL, &entry );
    Apic::enable_ioapic( IRQ_SYSTEM_CALL );
}

}     // namespace QuantumNEC::Kernel::x86_64
