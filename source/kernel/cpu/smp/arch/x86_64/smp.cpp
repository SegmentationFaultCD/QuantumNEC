#include <kernel/memory/heap/kheap/kheap_allocater.hpp>
#include <kernel/memory/page/page_walker.hpp>
#include <kernel/task/task.hpp>
#include <lib/spin_lock.hpp>
PUBLIC namespace QuantumNEC::Kernel {
    SymmetricMultiprocessing::SymmetricMultiprocessing( VOID ) noexcept {
        Lib::SpinLock lock { };
        using namespace QuantumNEC::Kernel;
        for ( auto i { 1ul }; i < __config.smp.cpu_count; ++i ) {
            lock.acquire( );
            __config.smp.cpus[ i ]->goto_address = apu_entry;
            auto stack_start                     = PageWalker { }.allocate< MemoryPageType::PAGE_4K >( 1 );
            Memory::gdt->tss[ i ].set_rsp0( (uint64_t)stack_start + PageAllocater::__page_size__< MemoryPageType::PAGE_4K > );
            lock.release( );
        }
        std::println< print_level::OK >( "Initialize Symmetric Multiprocessing" );
    }
    auto SymmetricMultiprocessing::send_IPI( IN Apic::InterruptCommandRegister icr ) -> VOID {
        Apic::write_apic( LOCAL_BASE_APIC_ICRL1, icr >> 32, Apic::ApicType::LOCAL_APIC );
        Apic::write_apic( LOCAL_BASE_APIC_ICRL0, icr & 0xffffffff, Apic::ApicType::LOCAL_APIC );
    }
    auto SymmetricMultiprocessing::switch_cpu( VOID ) -> VOID {
        Apic::InterruptCommandRegister icr;
        icr.vector                         = IRQ_SMP_SWITCH_CPU;
        icr.deliver_mode                   = APIC_ICR_IOAPIC_FIXED;
        icr.dest_mode                      = ICR_IOAPIC_DELV_PHYSICAL;
        icr.deliver_status                 = APIC_ICR_IOAPIC_IDLE;
        icr.level                          = ICR_LEVEL_DE_ASSERT;
        icr.trigger                        = APIC_ICR_IOAPIC_EDGE;
        icr.dest_shorthand                 = ICR_ALL_EXCLUDE_SELF;
        icr.destination.x2apic_destination = 0;
        send_IPI( icr );
    }
}