#include <kernel/cpu/cpu.hpp>
#include <kernel/global/arch/x86_64/global.hpp>
#include <kernel/interrupt/arch/x86_64/entry/entry_cascade_timer.hpp>
#include <kernel/interrupt/arch/x86_64/entry/idt.hpp>
#include <kernel/interrupt/arch/x86_64/pic/pic.hpp>
#include <kernel/print.hpp>
#include <kernel/task/task.hpp>
using namespace QuantumNEC;
PUBLIC namespace QuantumNEC::Kernel::x86_64 {
    _C_LINK auto save_current_frame( IN CONST InterruptDescriptorTable::InterruptFrame * frame ) -> VOID;

    auto CascadeTimerEntry::name( VOID ) noexcept -> VOID {
    }
    auto CascadeTimerEntry::error_code( [[maybe_unused]] uint64_t error_code ) noexcept -> VOID {
    }
    auto CascadeTimerEntry::handler( Frame * frame ) noexcept -> Frame * {
        Apic::eoi( frame->vector );
        Interrupt::global_jiffies++;

        CPU::switch_cpu( );

        if ( auto current = ProcessManager::get_running_task( ); current != ProcessManager::main_pcb ) {
            *ProcessManager::get_running_task( )->context.pcontext = *frame;
        }

        auto result = Scheduler { }.schedule( );
        if ( result.has_value( ) ) {
            std::println( "YEP!! " );
            if ( result.value( ) == ProcessManager::main_pcb ) {
                return frame;
            }
            else {
                return result.value( )->context.pcontext;
            }
        }
        else {
            std::println( "Error! {}", uint64_t( result.error( ) ) );
            while ( true );
        }
        // 在这里进行任务调度

        return frame;
    }
    auto CascadeTimerEntry::do_register( VOID ) -> VOID {
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
        std::println< print_level::DEBUG >( "S" );
    }
}