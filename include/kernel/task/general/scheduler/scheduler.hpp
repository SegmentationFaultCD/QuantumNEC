#pragma once
#include <kernel/task/general/scheduler/brain_fuck_scheduler/bfs.hpp>
#include <kernel/task/general/scheduler/brain_fuck_scheduler/bfs_helper.hpp>

namespace QuantumNEC::Kernel {
struct ProcessControlBlock;
using Scheduler       = BrainFuckScheduler< ProcessControlBlock >;
using SchedulerHelper = BrainFuckSchedulerHelper< ProcessControlBlock >;

// 针对各个调度器的推导指导
template < typename F >
scheduler_utils( F ) -> scheduler_utils< Scheduler >;

template < typename View, typename Function >
    requires std::invocable< Function, View >
auto operator|( View block, Function func ) {
    return func( block );
}

}     // namespace QuantumNEC::Kernel
#include <kernel/task/general/scheduler/brain_fuck_scheduler/bfs_interface.hpp>