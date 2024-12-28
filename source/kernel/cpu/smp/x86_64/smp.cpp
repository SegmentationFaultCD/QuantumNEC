#include <kernel/cpu/smp/x86_64/x86_64smp.hpp>
#include <kernel/memory/heap/kheap/kheap_allocater.hpp>
#include <kernel/memory/page/page_walker.hpp>
#include <kernel/task/task.hpp>
#include <lib/spin_lock.hpp>
namespace QuantumNEC::Kernel::x86_64 {
SymmetricMultiprocessing::SymmetricMultiprocessing( void ) noexcept {
    using namespace QuantumNEC::Kernel;
    using namespace std;
    for ( auto i { 1ul }; i < __config.smp.cpu_count; ++i ) {
        auto stack_start = PageWalker { }.allocate< MemoryPageType::PAGE_4K >( 1 );
        Memory::gdt->tss[ i ].set_rsp0( (uint64_t)stack_start + PageAllocater::__page_size__< MemoryPageType::PAGE_4K > );
        __config.smp.cpus[ i ]->goto_address = apu_entry;
    }
    println< print_level::OK >( "Initialize Symmetric Multiprocessing" );
}
auto SymmetricMultiprocessing::send_IPI( IN Apic::InterruptCommandRegister icr ) -> void {
    CPU::wrmsr( LOCAL_APIC_MSR_ICRL0, icr );
}
auto SymmetricMultiprocessing::switch_cpu( void ) -> void {
    Apic::InterruptCommandRegister icr { };
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
}     // namespace QuantumNEC::Kernel::x86_64