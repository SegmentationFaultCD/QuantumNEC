#include <kernel/memory/heap/kheap/kheap_walker.hpp>
#include <kernel/task/process/process.hpp>
namespace QuantumNEC::Kernel {
Process::Process( const char_t *name, uint64_t priority, void *entry, PCB::Flags::Type type ) noexcept {
    if ( type != PCB::Flags::Type::THREAD ) {
        PCB::Flags flags {
            .fpu_enable = PCB::Flags::Fpu::ENABLE,
            .fpu_used   = PCB::Flags::Fpu::USED,
            .task_type  = type,
        };
        this->pcb = new ( KHeapWalker { }.allocate( sizeof( PCB ) ) ) PCB {
            name,
            priority,
            flags,
            entry,
            0
        };
        this->has_inserted = false;
    }
}
Process::Process( Process &&process ) noexcept {
    this->pcb   = process.pcb;
    process.pcb = NULL;
}
auto Process::join( void ) noexcept -> void {
    Scheduler scheduler;
    if ( !this->has_inserted ) {
        scheduler.insert( this->pcb->schedule );
        this->has_inserted = true;
    }

    scheduler.wake_up( this->pcb->schedule );
}
}     // namespace QuantumNEC::Kernel