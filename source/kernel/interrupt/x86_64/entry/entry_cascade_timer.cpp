#include <kernel/cpu/cpu.hpp>
#include <kernel/global/x86_64/global.hpp>
#include <kernel/interrupt/interrupt.hpp>
#include <kernel/interrupt/x86_64/entry/entry_cascade_timer.hpp>
#include <kernel/interrupt/x86_64/entry/idt.hpp>
#include <kernel/interrupt/x86_64/pic/pic.hpp>
#include <kernel/interrupt/x86_64/pit/apic_timer.hpp>
#include <kernel/print.hpp>
#include <kernel/task/task.hpp>
using namespace QuantumNEC;
namespace QuantumNEC::Kernel::x86_64 {
_C_LINK auto save_current_frame( IN const InterruptDescriptorTable::InterruptFrame *frame ) -> void;

auto CascadeTimerEntry::name( void ) noexcept -> void {
}
auto CascadeTimerEntry::error_code( [[maybe_unused]] uint64_t error_code ) noexcept -> void {
}
auto CascadeTimerEntry::handler( Frame *frame ) noexcept -> Frame * {
    Apic::eoi( frame->vector );

    Interrupt::global_jiffies++;

    CPU::switch_cpu( );

    // *ProcessManager::get_running_task( )->context.pcontext = *( (PCB::ProcessContext *)frame );
    std::print( "{:x}\n", (void *)frame );

    auto result = Scheduler { }.schedule( );
    if ( result.has_value( ) ) {
        if ( result.value( ) == ProcessManager::main_pcb ) {
            return frame;
        }
        else {
            return result.value( )->context.pcontext;
        }
    }
    else {
        while ( true );
    }
    // 在这里进行任务调度

    return frame;
}
auto CascadeTimerEntry::do_register( void ) -> void {
    Apic::IOApicRedirectionEntry entry { };
    entry.vector         = IRQ_CASCADE_TIMER;
    entry.deliver_mode   = APIC_ICR_IOAPIC_FIXED;
    entry.dest_mode      = ICR_IOAPIC_DELV_PHYSICAL;
    entry.deliver_status = APIC_ICR_IOAPIC_IDLE;
    entry.polarity       = APIC_IOAPIC_POLARITY_HIGH;
    entry.irr            = APIC_IOAPIC_IRR_RESET;
    entry.trigger        = APIC_ICR_IOAPIC_EDGE;
    entry.mask           = APIC_ICR_IOAPIC_MASKED;
    Apic::install_ioapic( IRQ_CASCADE_TIMER, &entry );
    Apic::enable_ioapic( IRQ_CASCADE_TIMER );
}
}     // namespace QuantumNEC::Kernel::x86_64