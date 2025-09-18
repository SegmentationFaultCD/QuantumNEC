#include <kernel/interrupt/apic.hpp>
#include <kernel/interrupt/hpet.hpp>
#include <kernel/task/schedule/MuQss.hpp>
#include <kernel/task/task.hpp>
namespace Task {

auto MuQss::schedule( void ) -> void {
    // O(1)

    auto running_task = this->running_queue[ Interrupt::apic.apic_id( ) ].running_task;
    auto sched = running_task->schedule;
    if ( !sched->time_slice ) {
        // 时间片耗尽的情况

        sched->time_slice = this->rr_interval;
        sched->virtual_deadline = this->get_virtual_deadline( Interrupt::hpet->nano_time( ), this->get_prio_ratio( sched->nice ) );
        auto &running_queue = this->running_queue[ sched->cpu ];

        running_queue.lock->acquire( );

        // this->scheduler_queue[ sched->cpu ].insert( running_task, sched->virtual_deadline );
        while ( true );
        running_queue.lock->release( );

        auto best = *this->scheduler_queue[ Interrupt::apic.apic_id( ) ].begin( );

        for ( auto &cpu : this->running_queue ) {
            if ( !cpu.lock->try_lock( ) ) {
                continue;
            };

            auto &schedule_queue = this->scheduler_queue[ cpu.cpu_id ];
            auto p = *schedule_queue.begin( );

            if ( best == p ) {
                continue;
            }

            if ( p->schedule->virtual_deadline < best->schedule->virtual_deadline ) {
                best = p;
            }
            else {
                continue;
            }

            best->schedule->cpu = Interrupt::apic.apic_id( );

            schedule_queue.remove( best->schedule->virtual_deadline );

            this->running_queue[ best->schedule->cpu ].running_task = best;

            cpu.lock->release( );
        }
    }
    else {
        Display::println( "??? {}", Interrupt::apic.apic_id( ) );
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

    this->running_queue[ sched->cpu ].lock->acquire( );
    this->scheduler_queue[ sched->cpu ].insert( pcb, sched->virtual_deadline );
    Display::println( "{:x} {}", sched->virtual_deadline, (void *)this->scheduler_queue[ sched->cpu ].begin( )._pnode->data );
    Display::println( "{:x} {}", sched->virtual_deadline, (void *)( this->scheduler_queue[ sched->cpu ].search( sched->virtual_deadline )._pnode ) );
    this->running_queue.find( sched->cpu )->lock->release( );
}
auto MuQss::initialize_normal( PCB *pcb ) -> void {
    auto sched = pcb->schedule;
    // 时间片默认就是rr_interval值,但是可以改
    sched->time_slice = this->rr_interval;
    sched->cpu = Interrupt::apic.apic_id( );
    // nice可以在系统调用里面改
    sched->nice = this->default_nice;
    // prio_ratio的计算要参考nice, nice有40个, nice每次增加1,
    sched->virtual_deadline = this->get_virtual_deadline( Interrupt::hpet->nano_time( ), this->get_prio_ratio( this->default_nice ) );
    // 0 ~ 99 实时任务
    // 100 等时任务
    // 101 分时任务
    // 102 IDLE任务
    sched->priority = 101;
}
auto MuQss::remove( PCB * ) -> void {
}
}     // namespace Task