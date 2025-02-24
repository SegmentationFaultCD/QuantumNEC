#include <kernel/cpu/cpu.hpp>
#include <kernel/global/x86_64/global.hpp>
#include <kernel/interrupt/x86_64/entry/entry_syscall.hpp>
#include <kernel/interrupt/x86_64/entry/idt.hpp>
#include <kernel/interrupt/x86_64/pic/pic.hpp>
#include <kernel/print.hpp>
#include <kernel/syscall/ipc/receive.hpp>
#include <kernel/syscall/ipc/send.hpp>
#include <kernel/syscall/syscall.hpp>
using namespace QuantumNEC;
namespace QuantumNEC::Kernel::x86_64 {

auto SystemcallEntry::name( void ) noexcept -> void {
}
auto SystemcallEntry::error_code( [[maybe_unused]] uint64_t error_code ) noexcept -> void {
}
auto SystemcallEntry::handler( Frame *frame ) noexcept -> Frame * {
    Apic::eoi( frame->vector );
    if ( auto &servicer = Syscall::get_servicer( Syscall::Servicer( frame->regs.rax ) ); servicer.raw_control_block( ) ) {
        MessageSender sender;

        ProcessControlBlock::get_running_task( )->messages.change_message( 0, frame->regs.rdi );
        ProcessControlBlock::get_running_task( )->messages.change_message( 1, frame->regs.rsi );
        ProcessControlBlock::get_running_task( )->messages.change_message( 2, frame->regs.rdx );
        ProcessControlBlock::get_running_task( )->messages.change_message( 3, frame->regs.rcx );
        ProcessControlBlock::get_running_task( )->messages.change_message( 4, frame->regs.r8 );
        ProcessControlBlock::get_running_task( )->messages.change_message( 5, frame->regs.r9 );

        sender.role.sender.set_receiver( ProcessControlBlock::get_running_task( ), ProcessControlBlock::get_running_task( )->messages );

        sender.execute_order(uint64_t servicer_index, const message &messages)
    }
    return frame;
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
