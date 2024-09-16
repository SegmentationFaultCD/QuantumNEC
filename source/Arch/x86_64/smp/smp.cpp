#include <Arch/x86_64/platform/platform.hpp>
#include <Kernel/memory/memory.hpp>
#include <Lib/spin_lock.hpp>
#include <Kernel/task/pcb.hpp>
PUBLIC namespace QuantumNEC::Architecture {
    SymmetricMultiprocessing::SymmetricMultiprocessing( VOID ) noexcept {
        uint64_t stack_start = 0;
        Lib::SpinLock lock { };
        using namespace QuantumNEC::Kernel;
        for ( auto i { 0ul }; i < Architecture::__config.smp.cpu_count; ++i ) {
            lock.acquire( );
            __config.smp.cpus[ i ]->goto_address = apu_entry;
            stack_start = (uint64_t)Memory::page->malloc( TASK_STACK_SIZE / PAGE_4K_SIZE, MemoryPageType::PAGE_4K );
            
            lock.release( );
        }
    }
}