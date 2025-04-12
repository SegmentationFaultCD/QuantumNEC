#pragma once
#include <kernel/task/general/scheduler/brain_fuck_scheduler/bfs.hpp>
#include <kernel/task/general/scheduler/brain_fuck_scheduler/bfs_helper.hpp>

namespace QuantumNEC::Kernel {
class ProcessControlBlock;
using Scheduler       = BrainFuckScheduler< ProcessControlBlock >;
using SchedulerHelper = BrainFuckSchedulerHelper< ProcessControlBlock >;

}     // namespace QuantumNEC::Kernel
