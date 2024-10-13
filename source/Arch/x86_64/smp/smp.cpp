
#include <Arch/x86_64/platform/platform.hpp>
#include <Kernel/memory/memory.hpp>
#include <Kernel/task/task.hpp>
#include <Lib/spin_lock.hpp>

PUBLIC namespace QuantumNEC::Architecture {
    auto address = 0x200000ul;
    SymmetricMultiprocessing::SymmetricMultiprocessing( VOID ) noexcept {
        // uint64_t stack_start = 0;
        Lib::SpinLock lock { };
        using namespace QuantumNEC::Kernel;
        for ( auto i { 1 }; i < Architecture::__config.smp.cpu_count; ++i ) {
            lock.acquire( );
            __config.smp.cpus[ i ]->goto_address = apu_entry;
            auto stack_start = address;
            x86_64Architecture::gdt->tss[ i ].set_rsp0( stack_start + __page_allocater::__page_size< MemoryPageType::PAGE_4K > );
            address += __page_allocater::__page_size< MemoryPageType::PAGE_4K >;
            lock.release( );
        }
        std::println< std::ostream::HeadLevel::OK >( "Initialize Symmetric Multiprocessing" );
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