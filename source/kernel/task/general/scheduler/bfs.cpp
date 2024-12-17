#include <kernel/task/general/scheduler/bfs.hpp>
#include <kernel/task/process/process_manager.hpp>
namespace QuantumNEC::Kernel {
auto BrainFuckScheduler::__pick_next__( void ) -> std::expected< PCB *, ErrorCode > {
    // 针对不同优先级采用的pick next方法
    auto index = 0;

    for ( ; index < SchedulerHelper::total_priority; ++index ) {
        // 遍历队列查看是否有空
        if ( SchedulerHelper::bitmap[ index ] ) {
            break;
        }
    }
    if ( index > 102 ) {
        return std::unexpected { ErrorCode::ALL_QUEUE_ARE_EMPTY };
    }
    if ( index < 100 ) {
        // 实时任务，直接弹出首位
        return &( *SchedulerHelper::task_queue[ index ].begin( ) );
    }
    else {
        auto head = &SchedulerHelper::task_queue[ index ].begin( )->schedule.general_task_node;

        // EEVDF算法在剩余三个队列中整理并查找
        Lib::ListTable< PCB >::ListNode *p, *q, *tail;
        auto                             count = SchedulerHelper::task_queue[ index ].length( );

        for ( auto i = 0ul; i < count - 1; i++ ) {
            auto num = count - i - 1;
            q        = head;
            p        = q->next;
            tail     = head->prev;
            while ( num-- ) {
                if ( q->container->schedule.virtual_deadline < Interrupt::global_jiffies ) {
                    // deadline小于当前进程则直接弹出这个
                    return q->container;
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
        return head->container;
    }
    // 查找失败
    return std::unexpected { ErrorCode::ALL_QUEUE_ARE_EMPTY };
}

auto BrainFuckScheduler::__wake_up__( PCB *pcb ) -> PCB * {
    Interrupt::disable_interrupt( );
    SchedulerHelper::global_lock.acquire( );
    // 遍历运行队列查找是否可以抢占VD大于插入任务的CPU

    for ( auto &running_pcb : SchedulerHelper::running_queue ) {
        if ( running_pcb.schedule.virtual_deadline > pcb->schedule.virtual_deadline ) {
            // 不为空，说明有任务的VD比这个任务VS大
            // 那就抢占

            // 处理抢占的任务，将它归入所对应优先级的队列之中，并从运行队列中删除
            SchedulerHelper::running_queue.remove( running_pcb.schedule.general_task_node );
            SchedulerHelper::task_queue[ running_pcb.schedule.priority ].append( running_pcb.schedule.general_task_node );
            running_pcb.flags.state                                  = PCB::Flags::State::READY;
            SchedulerHelper::bitmap[ running_pcb.schedule.priority ] = true;
            // 处理抢占者，从任务队列中剔除，加入运行队列
            SchedulerHelper::task_queue[ running_pcb.schedule.priority ].remove( pcb->schedule.general_task_node );
            SchedulerHelper::running_queue.append( pcb->schedule.general_task_node );
            pcb->schedule.cpu_id = running_pcb.schedule.cpu_id;
            pcb->flags.state     = PCB::Flags::State::RUNNING;
            SchedulerHelper::global_lock.release( );
            Interrupt::enable_interrupt( );

            CPU::switch_cpu( );     // 切换CPU，在切换后进行换值
            return pcb;
            std::unreachable( );
        }
    }
    // 为空，说明这个要插入的任务VD太大了
    // 那就根据优先级插入任务等待队列
    // 插入队尾

    SchedulerHelper::bitmap[ pcb->schedule.priority ] = true;
    pcb->flags.state                                  = PCB::Flags::State::READY;
    SchedulerHelper::global_lock.release( );
    Interrupt::enable_interrupt( );
    return pcb;
}
// 任务睡眠
auto BrainFuckScheduler::__sleep__( IN uint64_t ticks ) -> std::expected< PCB *, ErrorCode > {
}

auto BrainFuckScheduler::__schedule__( ) -> std::expected< PCB *, ErrorCode > {
    for ( auto cpu_id = Interrupt::cpu_id( ); auto &pcb : SchedulerHelper::running_queue ) {
        if ( cpu_id == pcb.schedule.cpu_id ) {
            if ( pcb.schedule.jiffies ) {
                if ( ProcessManager::get_running_task( ) != &pcb ) {
                    pcb.activate( );
                }
                pcb.schedule.jiffies--;
                return &pcb;
            }
            else {
                // 时间片耗尽
                auto result = this->__pick_next__( );
                if ( result.has_value( ) ) {
                    auto new_pcb = result.value( );
                    SchedulerHelper::task_queue[ new_pcb->schedule.priority ].remove( new_pcb->schedule.general_task_node );
                    SchedulerHelper::running_queue.remove( pcb.schedule.general_task_node );
                    SchedulerHelper::task_queue[ pcb.schedule.priority ].append( pcb.schedule.general_task_node );
                    SchedulerHelper::bitmap[ pcb.schedule.priority ] = true;
                    pcb.flags.state                                  = PCB::Flags::State::READY;
                    pcb.close( );
                    // 重新计算VD，填充时间片
                    pcb.schedule.virtual_deadline = SchedulerHelper::make_virtual_deadline( pcb.schedule.priority );
                    pcb.schedule.jiffies          = SchedulerHelper::rr_interval;
                    new_pcb->flags.state          = PCB::Flags::State::RUNNING;
                    new_pcb->schedule.cpu_id      = pcb.schedule.cpu_id;
                    if ( SchedulerHelper::task_queue[ new_pcb->schedule.priority ].is_empty( ) ) {
                        SchedulerHelper::bitmap[ new_pcb->schedule.priority ] = false;
                    }
                    SchedulerHelper::running_queue.append( new_pcb->schedule.general_task_node );
                    new_pcb->activate( );
                    return new_pcb;
                }
            }
        }
    }
    return std::unexpected { ErrorCode::NO_TASK_CAN_SCHEDULER };
}
}     // namespace QuantumNEC::Kernel