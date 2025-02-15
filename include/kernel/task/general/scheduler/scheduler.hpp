#pragma once
#include <kernel/task/general/scheduler/brain_fuck_scheduler/bfs.hpp>
#include <kernel/task/general/scheduler/brain_fuck_scheduler/bfs_helper.hpp>

namespace QuantumNEC::Kernel {
struct ProcessControlBlock;
using Scheduler       = BrainFuckScheduler< ProcessControlBlock >;
using SchedulerHelper = BrainFuckSchedulerHelper< ProcessControlBlock >;

// inline auto scheduler_wake_up( Scheduler::block block ) -> Scheduler::block {
//     return block;
// }
// inline auto scheduler_remove( Scheduler::block block ) -> Scheduler::block {
//     return block;
// }
// inline auto scheduler_hang( Scheduler::block block ) -> Scheduler::block {
//     return block;
// }

//  requires conform_to_scheduler_standard< SchedulerType > &&

template < typename View, typename Function >
    requires std::invocable< Function, View >
auto operator|( View block, Function func ) {
    return func( block );
}

scheduler_inserter( BrainFuckScheduler< ProcessControlBlock >::view ( * )( BrainFuckScheduler< ProcessControlBlock >::view ) ) -> scheduler_inserter< Scheduler, BrainFuckScheduler< ProcessControlBlock >::view ( * )( BrainFuckScheduler< ProcessControlBlock >::view ) >;
scheduler_wakeuper( BrainFuckScheduler< ProcessControlBlock >::view ( * )( BrainFuckScheduler< ProcessControlBlock >::view ) ) -> scheduler_wakeuper< Scheduler, BrainFuckScheduler< ProcessControlBlock >::view ( * )( BrainFuckScheduler< ProcessControlBlock >::view ) >;
scheduler_hanger( BrainFuckScheduler< ProcessControlBlock >::view ( * )( BrainFuckScheduler< ProcessControlBlock >::view ) ) -> scheduler_hanger< Scheduler, BrainFuckScheduler< ProcessControlBlock >::view ( * )( BrainFuckScheduler< ProcessControlBlock >::view ) >;
scheduler_remover( BrainFuckScheduler< ProcessControlBlock >::view ( * )( BrainFuckScheduler< ProcessControlBlock >::view ) ) -> scheduler_remover< Scheduler, BrainFuckScheduler< ProcessControlBlock >::view ( * )( BrainFuckScheduler< ProcessControlBlock >::view ) >;

}     // namespace QuantumNEC::Kernel
#include <kernel/task/general/scheduler/brain_fuck_scheduler/bfs_interface.hpp>