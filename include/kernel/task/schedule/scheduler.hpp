#pragma once
#include <kernel/interrupt/idt.hpp>
#include <kernel/task/lock/spinlock.hpp>
#include <lib/rbtree.hpp>

namespace Task {
class MuQss;
class PCB;

struct Scheduler {
    struct CPU {
        PCB *running_task;
        s_locks *lock;
        std::uint64_t cpu_id;
        CPU( ) :
            running_task { }, lock { new s_locks {} } {}
        CPU( PCB *task ) :
            running_task { task }, lock { new s_locks {} } {}
        auto operator=( CPU &&c ) -> CPU & {
            this->running_task = c.running_task;
            c.running_task = nullptr;
            delete c.lock;
            return *this;
        }
        auto operator=( const CPU &c ) -> CPU & {
            this->running_task = c.running_task;
            return *this;
        }
        CPU( const CPU &cpu ) {
            this->lock = new s_locks;
            this->operator=( cpu );
        }
        CPU( CPU &&cpu ) {
            this->lock = new s_locks;
            this->operator=( std::move( cpu ) );
        }
        ~CPU( ) {
            delete this->lock;
        }
    };

    // interface
    virtual auto schedule( void ) -> void = 0;
    virtual auto sleep( PCB * ) -> void = 0;
    virtual auto wake_up( PCB * ) -> void = 0;
    virtual auto insert( PCB * ) -> void = 0;
    virtual auto remove( PCB * ) -> void = 0;
    // 任务运行队列
    Library::RBTree< std::uint64_t, CPU > running_queue;

    static auto initialize( void ) -> void;
    Scheduler( void ) = default;

    virtual ~Scheduler( void ) = default;

    virtual auto initialize_normal( PCB * ) -> void = 0;
};

inline Scheduler *scheduler;

}     // namespace Task