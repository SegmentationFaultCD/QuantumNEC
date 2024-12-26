#pragma once
#include <kernel/task/general/scheduler/brain_fuck_scheduler/bfs.hpp>
#include <kernel/task/general/scheduler/brain_fuck_scheduler/bfs_helper.hpp>
namespace QuantumNEC::Kernel {
struct PCB;
using Scheduler       = BrainFuckScheduler< PCB >;
using SchedulerHelper = BrainFuckSchedulerHelper< PCB >;
}     // namespace QuantumNEC::Kernel