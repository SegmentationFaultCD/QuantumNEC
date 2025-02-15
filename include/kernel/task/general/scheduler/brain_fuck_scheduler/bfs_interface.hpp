#pragma once
#include "./bfs.hpp"
namespace QuantumNEC::Kernel {

template < typename TaskControlBlock >
concept standard_pcb = requires { typename TaskControlBlock::template BrainFuckScheduler< TaskControlBlock >::Schedule; };

class ProcessControlBlock;
inline auto brain_fuck_scheduler_insert( typename BrainFuckScheduler< ProcessControlBlock >::view views ) {
    views.first.insert( views.second );
    return views;
}

inline auto brain_fuck_scheduler_wake_up( typename BrainFuckScheduler< ProcessControlBlock >::view views ) {
    views.first.wake_up( views.second );
    return views;
}

inline auto brain_fuck_scheduler_hang( typename BrainFuckScheduler< ProcessControlBlock >::view views ) {
    views.first.hang( views.second );
    return views;
}

inline auto brain_fuck_scheduler_remove( typename BrainFuckScheduler< ProcessControlBlock >::view views ) {
    views.first.remove( views.second );
    return views;
}

}     // namespace QuantumNEC::Kernel