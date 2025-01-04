#pragma once
#include <kernel/task/general/scheduler/brain_fuck_scheduler/bfs.hpp>
#include <kernel/task/general/scheduler/brain_fuck_scheduler/bfs_helper.hpp>
namespace QuantumNEC::Kernel {
struct PCB;
using Scheduler       = BrainFuckScheduler< PCB >;
using SchedulerHelper = BrainFuckSchedulerHelper< PCB >;

inline auto scheduler_insert( Scheduler &scheduler, Scheduler::Schedule &schedule ) -> Scheduler & {
    scheduler.insert( schedule );
    return scheduler;
}
inline auto scheduler_wake_up( Scheduler &scheduler, Scheduler::Schedule &schedule ) -> Scheduler & {
    scheduler.wake_up( schedule );
    return scheduler;
}
inline auto scheduler_remove( Scheduler &scheduler, Scheduler::Schedule &schedule ) -> Scheduler & {
    scheduler.remove( schedule );
    return scheduler;
}
inline auto scheduler_hang( Scheduler &scheduler, Scheduler::Schedule &schedule ) -> Scheduler & {
    scheduler.hang( schedule );
    return scheduler;
}

auto operator|( auto &&L, auto &&R )
    requires std::invocable< decltype( R ), decltype( L ) > && conform_to_scheduler_standard< decltype( L ) >
{
    return R( L );
}
}     // namespace QuantumNEC::Kernel