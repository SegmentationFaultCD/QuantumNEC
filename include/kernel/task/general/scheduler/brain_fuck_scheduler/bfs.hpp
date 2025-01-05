#pragma once

#include <kernel/cpu/cpu.hpp>
#include <kernel/interrupt/interrupt.hpp>
#include <kernel/memory/memory.hpp>
#include <kernel/print.hpp>
#include <kernel/task/general/scheduler/interface.hpp>
#include <lib/Uefi.hpp>
#include <lib/list.hpp>
#include <lib/spin_lock.hpp>
#include <utility>

namespace QuantumNEC::Kernel {

// 该调度器思想由Con Kolivas发明
// 面向桌面端设备，适用与较少的CPU
template < typename PCB >
class BrainFuckSchedulerHelper;
template < typename PCB >
class BrainFuckScheduler : public SchedulerInterface {
    friend BrainFuckSchedulerHelper< PCB >;
    using helper = BrainFuckSchedulerHelper< PCB >;
    using self   = BrainFuckScheduler< PCB >;

public:
    struct Schedule {
        uint64_t                        jiffies;               // 可运行的时间片
        uint64_t                        priority;              // 任务优先级
        uint64_t                        virtual_deadline;      // 虚拟截止时间
        uint64_t                        cpu_id;                // CPU ID
        Lib::ListTable< PCB >::ListNode general_task_node;     // 通用任务队列 连接除running状态的每个任务
        uint64_t                        signal;                // 任务持有的信号
        enum class State : uint64_t {
            RUNNING   = 0,
            READY     = 1,
            BLOCKED   = 2,
            SENDING   = 3,
            RECEIVING = 4,
            WAITING   = 5,
            HANGING   = 6,
            DIED      = 7,
        } state : 7;     // 任务状态
    };

public:
    enum class ErrorCode {
        ALL_QUEUE_ARE_EMPTY,
        CAN_NOT_INSERT_TASK,
        NO_TASK_CAN_SCHEDULER
    };

public:
    using view = std::pair< self &, Schedule & >;

    auto operator|( Schedule &schedule ) -> view {
        return { *this, schedule };
    }

public:
    enum Priority {
        // 0 ~ 99 为实时任务
        ISO          = 100,     // 等时任务
        NORMAL_BATCH = 101,     // 分时任务，普通任务
        IDLEPRIO     = 102,     // CPU 即将处于 IDLE 状态时才被调度的进程，低优先级任务
    };

public:
    explicit BrainFuckScheduler( void ) noexcept = default;
    ~BrainFuckScheduler( void )                  = default;

public:
    friend SchedulerInterface;

private:
    auto __insert__( Schedule &schedule ) -> Schedule * {
        Interrupt::disable_interrupt( );
        helper::global_lock.acquire( );
        // 根据优先级插入任务等待队列队尾
        helper::task_queue[ schedule.priority ].append( schedule.general_task_node );
        helper::bitmap[ schedule.priority ] = true;
        schedule.state                      = Schedule::State::READY;
        helper::global_lock.release( );
        Interrupt::enable_interrupt( );
        return &schedule;
    }

    auto __pick_next__( void ) -> std::expected< Schedule *, ErrorCode > {
        // 针对不同优先级采用的pick next方法
        auto index = 0;

        for ( ; index < helper::total_priority; ++index ) {
            // 遍历队列查看是否有空
            if ( helper::bitmap[ index ] ) {
                break;
            }
        }
        if ( index > 102 ) {
            return std::unexpected { ErrorCode::ALL_QUEUE_ARE_EMPTY };
        }
        if ( index < 100 ) {
            // 实时任务，直接弹出首位

            return &( *helper::task_queue[ index ].begin( ) ).schedule;
        }
        else {
            auto head = &helper::task_queue[ index ].begin( )->schedule.general_task_node;

            // EEVDF算法在剩余三个队列中整理并查找
            typename Lib::ListTable< PCB >::ListNode *p, *q, *tail;
            auto                                      count = helper::task_queue[ index ].length( );

            for ( auto i = 0ul; i < count - 1; i++ ) {
                auto num = count - i - 1;
                q        = head;
                p        = q->next;
                tail     = head->prev;
                while ( num-- ) {
                    if ( q->container->schedule.virtual_deadline < Interrupt::global_jiffies ) {
                        // deadline小于当前进程则直接弹出这个

                        return &q->container->schedule;
                    }
                    if ( q->container->schedule.virtual_deadline > p->container->schedule.virtual_deadline )     // 如果该结点的值大于后一个结点，则交换
                    {
                        q->next    = p->next;
                        p->next    = q;
                        tail->next = p;
                    }
                    // 进行指针的移动
                    tail = tail->next;
                    q    = tail->next;
                    p    = q->next;
                }
            }

            // 全部整理一遍后，弹出头
            return &head->container->schedule;
        }
        // 查找失败

        return std::unexpected { ErrorCode::ALL_QUEUE_ARE_EMPTY };
    }
    // 任务唤醒
    auto __wake_up__( Schedule &schedule ) -> Schedule * {
        Interrupt::disable_interrupt( );
        helper::global_lock.acquire( );
        // 遍历运行队列查找是否可以抢占VD大于插入任务的CPU
        for ( auto &running_pcb : helper::running_queue ) {
            if ( running_pcb.schedule.virtual_deadline > schedule.virtual_deadline ) {
                // 不为空，说明有任务的VD比这个任务VS大
                // 那就抢占

                // 处理抢占的任务，将它归入所对应优先级的队列之中，并从运行队列中删除
                helper::running_queue.remove( running_pcb.schedule.general_task_node );

                helper::task_queue[ running_pcb.schedule.priority ].append( running_pcb.schedule.general_task_node );

                running_pcb.schedule.state = Schedule::State::READY;

                helper::bitmap[ running_pcb.schedule.priority ] = true;

                // 处理抢占者，从任务队列中剔除，加入运行队列
                helper::task_queue[ schedule.priority ].remove( schedule.general_task_node );

                if ( helper::task_queue[ schedule.priority ].is_empty( ) ) {
                    helper::bitmap[ schedule.priority ] = false;
                }
                helper::running_queue.append( schedule.general_task_node );

                schedule.cpu_id = running_pcb.schedule.cpu_id;
                schedule.state  = Schedule::State::RUNNING;
                helper::global_lock.release( );
                Interrupt::enable_interrupt( );
                CPU::switch_cpu( );     // 切换CPU，在切换后进行换值

                return &schedule;
            }
        }

        // the virtual deadline is too big, none of running tasks can be respaced.
        helper::global_lock.release( );
        Interrupt::enable_interrupt( );
        return &schedule;
    }
    // Task hang
    auto __hang__( Schedule & ) -> void {
        std::unreachable( );
    }
    // 任务调度
    auto __schedule__( void ) -> std::expected< Schedule *, ErrorCode > {
        helper::global_lock.acquire( );
        for ( auto cpu_id = Interrupt::cpu_id( ); auto &pcb : helper::running_queue ) {
            if ( cpu_id == pcb.schedule.cpu_id ) {
                if ( pcb.schedule.jiffies ) {
                    if ( PCB::get_running_task( ) != &pcb ) {
                        static_assert( std::invocable< decltype( PCB::get_running_task ) > );
                        pcb.activate( );
                    }
                    pcb.schedule.jiffies--;
                    helper::global_lock.release( );
                    return &pcb.schedule;
                }
                else {
                    // 时间片耗尽
                    auto result = this->__pick_next__( );
                    if ( result.has_value( ) ) {
                        auto new_pcb = result.value( );

                        helper::task_queue[ new_pcb->priority ].remove( new_pcb->general_task_node );

                        helper::running_queue.remove( pcb.schedule.general_task_node );
                        helper::task_queue[ pcb.schedule.priority ].append( pcb.schedule.general_task_node );
                        helper::bitmap[ pcb.schedule.priority ] = true;
                        pcb.schedule.state                      = Schedule::State::READY;

                        pcb.close( );

                        // 重新计算VD，填充时间片
                        pcb.schedule.virtual_deadline = helper::make_virtual_deadline( pcb.schedule.priority );
                        pcb.schedule.jiffies          = helper::rr_interval;
                        new_pcb->state                = Schedule::State::RUNNING;
                        new_pcb->cpu_id               = pcb.schedule.cpu_id;
                        if ( helper::task_queue[ new_pcb->priority ].is_empty( ) ) {
                            helper::bitmap[ new_pcb->priority ] = false;
                        }
                        helper::running_queue.append( new_pcb->general_task_node );
                        new_pcb->general_task_node.container->activate( );
                        helper::global_lock.release( );

                        return new_pcb;
                    }
                    else {
                        // TODO fault handler
                    }
                }
            }
        }
        helper::global_lock.release( );
        return std::unexpected { ErrorCode::NO_TASK_CAN_SCHEDULER };
    }
    // Task remove
    auto __remove__( Schedule & ) -> void {
    }
};

}     // namespace QuantumNEC::Kernel
