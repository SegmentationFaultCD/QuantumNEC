
#include <Arch/x86_64/platform/platform.hpp>
#include <Kernel/memory/memory.hpp>
#include <Kernel/memory/task.hpp>
#include <Lib/spin_lock.hpp>

PUBLIC namespace QuantumNEC::Architecture {
    SymmetricMultiprocessing::SymmetricMultiprocessing( VOID ) noexcept {
        uint64_t stack_start = 0;
        Lib::SpinLock lock { };
        using namespace QuantumNEC::Kernel;
        for ( auto i { 0ul }; i < Architecture::__config.smp.cpu_count; ++i ) {
            lock.acquire( );
            __config.smp.cpus[ i ]->goto_address = apu_entry;
            stack_start = (uint64_t)Memory::page->allocate( TASK_STACK_SIZE / PAGE_4K_SIZE, MemoryPageType::PAGE_4K );

            lock.release( );
        }
    }
    auto SymmetricMultiprocessing::send_IPI( IN Apic::InterruptCommandRegister icr ) -> VOID {
        // icr.deliver_mode = APIC_ICR_IOAPIC_FIXED;
        // icr.dest_mode = ICR_IOAPIC_DELV_PHYSICAL;
        // icr.deliver_status = APIC_ICR_IOAPIC_IDLE;
        // icr.level = ICR_LEVEL_DE_ASSERT;
        // icr.trigger = APIC_ICR_IOAPIC_EDGE;
        // icr.dest_shorthand = ICR_ALL_EXCLUDE_SELF;
        // icr.destination.x2apic_destination = 0x00;
        CPUs::wrmsr( LOCAL_APIC_MSR_ICRLO, icr );
        CPUs::wrmsr( LOCAL_APIC_MSR_ICRL1, icr >> 32 );
    }
}