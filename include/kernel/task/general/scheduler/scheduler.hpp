#pragma once
#include <kernel/task/general/scheduler/brain_fuck_scheduler/bfs.hpp>
#include <kernel/task/general/scheduler/brain_fuck_scheduler/bfs_helper.hpp>

namespace QuantumNEC::Kernel {
struct PCB;
using Scheduler       = BrainFuckScheduler< PCB >;
using SchedulerHelper = BrainFuckSchedulerHelper< PCB >;

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

scheduler_inserter( BrainFuckScheduler< PCB >::view ( * )( BrainFuckScheduler< PCB >::view ) ) -> scheduler_inserter< Scheduler, BrainFuckScheduler< PCB >::view ( * )( BrainFuckScheduler< PCB >::view ) >;
scheduler_wakeuper( BrainFuckScheduler< PCB >::view ( * )( BrainFuckScheduler< PCB >::view ) ) -> scheduler_wakeuper< Scheduler, BrainFuckScheduler< PCB >::view ( * )( BrainFuckScheduler< PCB >::view ) >;
scheduler_hanger( BrainFuckScheduler< PCB >::view ( * )( BrainFuckScheduler< PCB >::view ) ) -> scheduler_hanger< Scheduler, BrainFuckScheduler< PCB >::view ( * )( BrainFuckScheduler< PCB >::view ) >;
scheduler_remover( BrainFuckScheduler< PCB >::view ( * )( BrainFuckScheduler< PCB >::view ) ) -> scheduler_remover< Scheduler, BrainFuckScheduler< PCB >::view ( * )( BrainFuckScheduler< PCB >::view ) >;

}     // namespace QuantumNEC::Kernel
#include <kernel/task/general/scheduler/brain_fuck_scheduler/bfs_interface.hpp>