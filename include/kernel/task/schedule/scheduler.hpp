#pragma once
#include <kernel/interrupt/idt.hpp>
#include <lib/vector>
namespace Task {
template < typename PCB >
struct Scheduler {
    // interface
    virtual auto schedule( void ) -> Interrupt::IDT::Frame * = 0;
    virtual auto sleep( PCB * ) -> void = 0;
    virtual auto wake_up( PCB * ) -> void = 0;
    // 任务运行队列
    std::cxxvector< PCB * > running_queue;
};

auto initialize_scheduler( void ) -> void;

}     // namespace Task