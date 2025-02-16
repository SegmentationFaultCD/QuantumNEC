#pragma once
#include "../interface.hpp"
#include "./bfs.hpp"
namespace QuantumNEC::Kernel {

template < typename TaskControlBlock >
concept standard_pcb = requires { typename TaskControlBlock::template BrainFuckScheduler< TaskControlBlock >::Schedule; };

class ProcessControlBlock;
inline auto brain_fuck_scheduler_insert( BrainFuckScheduler< ProcessControlBlock >::view views ) {
    views.first.insert( views.second );
    return views;
}

inline auto brain_fuck_scheduler_wake_up( BrainFuckScheduler< ProcessControlBlock >::view views ) {
    views.first.wake_up( views.second );
    return views;
}

inline auto brain_fuck_scheduler_hang( BrainFuckScheduler< ProcessControlBlock >::view views ) {
    views.first.hang( views.second );
    return views;
}

inline auto brain_fuck_scheduler_remove( BrainFuckScheduler< ProcessControlBlock >::view views ) {
    views.first.remove( views.second );
    return views;
}

}     // namespace QuantumNEC::Kernel