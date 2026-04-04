#include <kernel/driver/cpu/io.hpp>
#include <kernel/interrupt/apic.hpp>
#include <kernel/interrupt/hpet.hpp>
#include <kernel/task/schedule/Muqss.hpp>
#include <kernel/task/schedule/scheduler.hpp>
#include <kernel/task/task.hpp>
#include <lib/vector>
namespace Task {

auto Muqss::schedule( void ) -> void {
    auto &current = scheduler->get_current( );
    auto &running_task = current.core.running_task;
    auto &sched = running_task.schedule;

    /*
     * 1 时间耗尽+next不存在任务
     * 2 时间耗尽+next存在任务 （-）
     * 3 时间未耗尽+next存在任务 （-）
     * 4 时间未耗尽+next不存在任务（-）
     */

    ( [ & ] {
        auto pop_running_tasks = [ & ] {
            if ( current.core.running_task.schedule->priority <= std::to_underlying( Schedule::Priority::ISO ) ) {
                current.core.RT_task_queue.push( std::move( current.core.running_task ) );
            }
            else {
                current.core.scheduler_queue.push( std::move( current.core.running_task ) );
            }
        };

        if ( !sched->time_slice ) {
            sched->time_slice = this->rr_interval;
            sched->virtual_deadline = this->get_virtual_deadline( Interrupt::hpet->nano_time( ), this->get_prio_ratio( sched->nice ) );

            if ( current.core.next.has_task( ) ) {
                // 抢占了
                pop_running_tasks( );
                return;
            }

            current.core.lock->lock( );
            if ( !current.core.RT_task_queue.empty( ) ) {
                current.core.next = std::move( current.core.RT_task_queue.front( ) );
                current.core.RT_task_queue.pop( );
                current.core.next.schedule->cpu = current.core.cpu_id;
                pop_running_tasks( );
                current.core.lock->unlock( );
                return;
            }
            current.core.lock->unlock( );

            if ( interactive ) {
                std::priority_queue< const PCB *, std::cxxvector< const PCB * >, mCore::compare_ptr > sq;
                // 我们只对第一个任务感兴趣,并且在只有少量任务的情况下可以允许特判

                for ( auto &cpu : scheduler->running_queue ) {
                    if ( cpu.core.cpu_id == current.core.cpu_id ) {
                        continue;
                    }
                    if ( !cpu.core.scheduler_queue.empty( ) ) {
                        sq.push( &cpu.core.scheduler_queue.top( ) );
                    }
                }

                current.core.lock->lock( );
                while ( !sq.empty( ) ) {
                    if ( auto &rq = scheduler->running_queue[ sq.top( )->schedule->cpu ]; rq.core.lock->try_lock( ) ) {
                        current.core.next = const_cast< PCB && >( rq.core.scheduler_queue.top( ) );
                        rq.core.scheduler_queue.pop( );
                        current.core.next.schedule->cpu = current.core.cpu_id;
                        pop_running_tasks( );

                        rq.core.lock->unlock( );
                        current.core.lock->unlock( );
                        return;
                    }
                    sq.pop( );
                }
                current.core.lock->unlock( );
            }
            current.core.lock->lock( );
            if ( !current.core.scheduler_queue.empty( ) ) {
                current.core.next = const_cast< PCB && >( current.core.scheduler_queue.top( ) );
                current.core.scheduler_queue.pop( );
                current.core.next.schedule->cpu = current.core.cpu_id;
                pop_running_tasks( );
            }
            current.core.lock->unlock( );

            // 1全为空（基本不可能）
            // 2根本没找到任务,换句话说任务全用光了<=> 别的核心正在争夺所有的任务
        }
        else {
            sched->time_slice--;
        }
    } )( );

    current.core.lock->lock( );
    if ( current.core.next.has_task( ) ) {
        current.core.next.activate( );
        current.core.running_task = std::move( current.core.next );
    }
    current.core.lock->unlock( );
}

auto Muqss::mCore::compare::operator( )( const PCB &x, const PCB &y ) const -> bool {
    if ( auto result = x.schedule->priority <=> y.schedule->priority; result != 0 ) {
        return result > 0;
    }
    else {
        return x.schedule->virtual_deadline > y.schedule->virtual_deadline;
    }
}
auto Muqss::mCore::compare_ptr::operator( )( const PCB *x, const PCB *y ) const -> bool {
    if ( auto result = x->schedule->priority <=> y->schedule->priority; result != 0 ) {
        return result > 0;
    }
    else {
        return x->schedule->virtual_deadline > y->schedule->virtual_deadline;
    }
}

auto Muqss::yield( ) -> void {
    auto &current = scheduler->get_current( );

    // 一般来说都是运行时的任务主动休眠, 并且不会是实时任务
    current.core.lock->lock( );

    if ( !current.core.RT_task_queue.empty( ) ) {
        current.core.next = std::move( current.core.RT_task_queue.front( ) );
        current.core.RT_task_queue.pop( );
        current.core.next.schedule->cpu = current.core.cpu_id;
    }
    else if ( !current.core.scheduler_queue.empty( ) ) {
        current.core.next = const_cast< PCB && >( current.core.scheduler_queue.top( ) );
        current.core.scheduler_queue.pop( );
        current.core.next.schedule->cpu = current.core.cpu_id;
    }

    if ( current.core.running_task.schedule->priority <= std::to_underlying( Schedule::Priority::ISO ) ) {
        current.core.RT_task_queue.push( std::move( current.core.running_task ) );
    }
    else {
        current.core.scheduler_queue.push( std::move( current.core.running_task ) );
    }

    current.core.lock->unlock( );
    // 没任务可以切换了
}

auto Muqss::wake_up( PCB &&pcb ) -> void {
    std::priority_queue< const PCB *, std::cxxvector< const PCB * >, mCore::compare_ptr > rq;
    for ( auto &cpu : scheduler->running_queue ) {
        rq.push( &cpu.core.running_task );
    }

    while ( !rq.empty( ) ) {
        auto p = rq.top( );
        // priority越大优先级越低
        if ( pcb.schedule->priority > p->schedule->priority ) {
            continue;
        }

        auto &q = scheduler->running_queue[ p->schedule->cpu ];
        if ( pcb.schedule->priority < p->schedule->priority || pcb.schedule->virtual_deadline < p->schedule->virtual_deadline && q.core.lock->try_lock( ) ) {
            q.core.next = std::move( pcb );
            q.core.next.schedule->cpu = q.core.cpu_id;
            q.core.lock->unlock( );
            return;
        }
        rq.pop( );
    }

    auto &current = scheduler->get_current( );
    current.core.lock->lock( );
    if ( pcb.schedule->priority <= std::to_underlying( Schedule::Priority::ISO ) ) {
        current.core.RT_task_queue.push( std::move( pcb ) );
    }
    else {
        current.core.scheduler_queue.push( std::move( pcb ) );
    }
    current.core.lock->unlock( );
}

auto Muqss::first_initialize( PCB &pcb ) -> void {
    auto &sched = pcb.schedule;
    // 0 ~ 99 实时任务
    // 100 等时任务
    // 101 分时任务
    // 102 IDLE任务
    using enum Schedule::Priority;
    sched->priority = std::to_underlying( NORMAL );     // 临时顶替一下

    // 时间片默认就是rr_interval值,但是可以改
    sched->time_slice = this->rr_interval;
    sched->cpu = scheduler->get_current( ).core.cpu_id;

    // prio_ratio的计算要参考nice, nice有40个, nice每次增加1,
    // nice可以在系统调用里面改
    sched->nice = this->default_nice;
    sched->virtual_deadline = this->get_virtual_deadline( Interrupt::hpet->nano_time( ), this->get_prio_ratio( this->default_nice ) );
}
}     // namespace Task