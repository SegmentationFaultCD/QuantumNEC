#include <algorithm>
#include <kernel/driver/cpu/io.hpp>
#include <kernel/interrupt/apic.hpp>
#include <kernel/interrupt/hpet.hpp>
#include <kernel/task/schedule/MuQss.hpp>
#include <kernel/task/task.hpp>
#include <lib/vector>
namespace Task {

auto MuQss::schedule( void ) -> void {
    // O(1)

    auto running_task = this->running_queue[ Interrupt::apic.apic_id( ) ].running_task;

    auto sched = running_task->schedule;
    if ( !sched->time_slice ) {
        // 时间片耗尽的情况

        using T = std::pair< std::uint64_t, PCB * >;
        std::cxxvector< T > sq;

        for ( auto &cpu : this->running_queue ) {
            auto &sched_queue = this->scheduler_queue[ cpu.cpu_id ];
            if ( sched_queue.is_empty( ) ) {
                continue;
            }
            sq.emplace_back( std::pair { cpu.cpu_id, *sched_queue.begin( ) } );
        }

        std::ranges::sort( sq, []( const T &a, const T &b ) { return a.second->schedule->virtual_deadline > b.second->schedule->virtual_deadline; } );

        bool get_task = false;
        for ( auto &i : sq ) {
            if ( auto lock = running_queue[ i.first ].lock; lock->try_lock( ) ) {
                auto &rq = running_queue[ i.first ];
                auto *p = i.second;

                get_task = true;
                this->scheduler_queue[ i.first ].remove( p->schedule->virtual_deadline );
                p->schedule->cpu = Interrupt::apic.apic_id( );
                this->running_queue[ p->schedule->cpu ].running_task = p;
                this->scheduler_queue[ i.first ].insert( running_task, sched->virtual_deadline );
                lock->unlock( );
                return;
            }
        }
        if ( !get_task ) {
            // 到这里说明根本没找到任务,换句话说任务全用光了
            // 其实根本没可能(划掉)
            sched->time_slice = this->rr_interval;
            sched->virtual_deadline = this->get_virtual_deadline( Interrupt::hpet->nano_time( ), this->get_prio_ratio( this->default_nice ) );
            return;
        }
    }
    else {
        sched->time_slice--;
    }
}
auto MuQss::sleep( PCB * ) -> void {
}
auto MuQss::wake_up( PCB * ) -> void {
}
auto MuQss::insert( PCB *pcb ) -> void {
    this->initialize_normal( pcb );
    auto sched = pcb->schedule;
    this->scheduler_queue[ sched->cpu ].insert( pcb, sched->virtual_deadline );
}
auto MuQss::initialize_normal( PCB *pcb ) -> void {
    auto sched = pcb->schedule;

    // 0 ~ 99 实时任务
    // 100 等时任务
    // 101 分时任务
    // 102 IDLE任务

    sched->priority = 101;     // 临时顶替一下

    // 时间片默认就是rr_interval值,但是可以改
    sched->time_slice = this->rr_interval;
    sched->cpu = Interrupt::apic.apic_id( );

    if ( sched->priority <= 100 ) {
        this->RT_task_queue.emplace_back( pcb );
    }
    else {
        // prio_ratio的计算要参考nice, nice有40个, nice每次增加1,
        // nice可以在系统调用里面改
        sched->nice = this->default_nice;
        sched->virtual_deadline = this->get_virtual_deadline( Interrupt::hpet->nano_time( ), this->get_prio_ratio( this->default_nice ) );
    }
}
auto MuQss::remove( PCB * ) -> void {
}
}     // namespace Task