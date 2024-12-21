#include <kernel/memory/heap/kheap/kheap_walker.hpp>
#include <kernel/task/process/process_creater.hpp>
#include <kernel/task/task.hpp>
namespace QuantumNEC::Kernel {
auto ProcessCreater::create( const char_t *name, uint64_t priority, void *entry, PCB::Flags::Type type ) -> std::expected< PCB *, ErrorCode > {
    if ( type == PCB::Flags::Type::THREAD ) {
        return std::unexpected { ErrorCode::ERROR_TASK_TYPE };
    }
    PCB::Flags flags {
        .state      = PCB::Flags::State::READY,
        .fpu_enable = PCB::Flags::Fpu::ENABLE,
        .fpu_used   = PCB::Flags::Fpu::USED,
        .task_type  = type,
    };
    auto pcb = new ( KHeapWalker { }.allocate( sizeof( PCB ) ) ) PCB {
        name,
        priority,
        flags,
        entry,
        0
    };

    if ( !pcb ) {
        return std::unexpected { ErrorCode::ALLOCATE_MEMORY_FAULT };
    }
    Scheduler scheduler;
    scheduler.insert( pcb );
    return pcb;
}
}     // namespace QuantumNEC::Kernel