#include <kernel/memory/heap/kheap/kheap_walker.hpp>
#include <kernel/task/process/process_creater.hpp>
#include <kernel/task/task.hpp>
PUBLIC namespace QuantumNEC::Kernel {
    auto ProcessCreater::create( CONST char_t * _name_, uint64_t _priority_, VOID * _entry_, PCB::Type type ) -> std::expected< PCB *, ErrorCode > {
        if ( type == PCB::Type::THREAD ) {
            return std::unexpected { ErrorCode::ERROR_TASK_TYPE };
        }
        PCB::Flags flags {
            .state      = PCB::State::READY,
            .task_type  = type,
            .fpu_used   = PCB::FpuUsed::USED,
            .fpu_enable = PCB::FpuEnable::ENABLE,
            .red        = 0
        };
        auto pcb = new ( KHeapWalker { }.allocate( sizeof( PCB ) ) ) PCB {
            _name_,
            _priority_,
            flags,
            _entry_,
            0
        };
        if ( !pcb ) {
            return std::unexpected { ErrorCode::ALLOCATE_MEMORY_FAULT };
        }
        return Scheduler { }.wake_up( pcb );
    }
}