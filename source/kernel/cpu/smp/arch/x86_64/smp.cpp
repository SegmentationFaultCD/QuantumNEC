#include <kernel/memory/heap/kheap/kheap_allocater.hpp>
#include <kernel/memory/memory.hpp>
#include <kernel/task/task.hpp>
#include <lib/spin_lock.hpp>
PUBLIC namespace QuantumNEC::Kernel {
    SymmetricMultiprocessing::SymmetricMultiprocessing( VOID ) noexcept {
        // uint64_t stack_start = 0;
        Lib::SpinLock lock { };
        using namespace QuantumNEC::Kernel;
        for ( auto i { 1ul }; i < __config.smp.cpu_count; ++i ) {
            lock.acquire( );
            __config.smp.cpus[ i ]->goto_address = apu_entry;
            auto stack_start                     = KHeapAllocater { }.allocate( PageAllocater::__page_size__< MemoryPageType::PAGE_4K > );
            Memory::gdt->tss[ i ].set_rsp0( (uint64_t)stack_start + PageAllocater::__page_size__< MemoryPageType::PAGE_4K > );

            lock.release( );
        }
        // std::println< std::ostream::HeadLevel::OK >( "Initialize Symmetric Multiprocessing" );
    }
    auto SymmetricMultiprocessing::send_IPI( IN Apic::InterruptCommandRegister icr ) -> VOID {
        // icr.deliver_mode = APIC_ICR_IOAPIC_FIXED;
        // icr.dest_mode = ICR_IOAPIC_DELV_PHYSICAL;
        // icr.deliver_status = APIC_ICR_IOAPIC_IDLE;
        // icr.level = ICR_LEVEL_DE_ASSERT;
        // icr.trigger = APIC_ICR_IOAPIC_EDGE;
        // icr.dest_shorthand = ICR_ALL_EXCLUDE_SELF;
        // icr.destination.x2apic_destination = 0x00;
        CPU::wrmsr( LOCAL_APIC_MSR_ICRLO, icr );
        CPU::wrmsr( LOCAL_APIC_MSR_ICRL1, icr >> 32 );
    }
}