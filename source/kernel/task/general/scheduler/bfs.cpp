#include <kernel/task/general/scheduler/bfs.hpp>
PUBLIC namespace QuantumNEC::Kernel {
    auto BrainFuckScheduler::__pick_next__( VOID ) -> std::expected< PCB *, ErrorCode > {
        // 针对不同优先级采用的pick next方法
        auto index = 0;

        for ( ; index < SchedulerHelper::total_priority; ++index ) {
            // 遍历队列查看是否有空
            if ( SchedulerHelper::bitmap[ index ] ) {
                break;
            }
        }
        if ( index < 100 ) {
            // 实时任务，直接弹出首位

            return &( *SchedulerHelper::task_queue[ index ].begin( ) );
        }
        else {
            for ( auto i = index; i < SchedulerHelper::total_priority; ++i ) {
                if ( SchedulerHelper::task_queue[ i ].is_empty( ) ) {
                    continue;
                }
                auto head = &SchedulerHelper::task_queue[ i ].begin( )->general_task_node;

                // EEVDF算法在剩余三个队列中整理并查找
                Lib::ListTable< PCB >::ListNode *p, *q, *tail;
                auto                             count = SchedulerHelper::task_queue[ i ].length( );

                for ( auto i = 0ul; i < count - 1; i++ ) {
                    auto num = count - i - 1;
                    q        = head;
                    p        = q->next;
                    tail     = head->prev;

                    while ( num-- ) {
                        if ( auto pcb = ( (PCB *)q->container ); pcb->virtual_deadline < Interrupt::global_jiffies ) {
                            // deadline小于当前进程则直接弹出这个

                            return pcb;
                        }
                        if ( ( (PCB *)q->container )->virtual_deadline > ( (PCB *)p->container )->virtual_deadline )     // 如果该结点的值大于后一个结点，则交换
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
                return (PCB *)head->container;
            }
        }
        // 查找失败
        return std::unexpected { ErrorCode::ALL_QUEUE_ARE_EMPTY };
    }

    auto BrainFuckScheduler::__wake_up__( PCB * pcb ) -> PCB * {
        Interrupt::disable_interrupt( );
        SchedulerHelper::global_lock.acquire( );
        // 遍历运行队列查找是否可以抢占VD大于插入任务的CPU

        for ( auto &running_pcb : SchedulerHelper::running_queue ) {
            if ( running_pcb.virtual_deadline > pcb->virtual_deadline ) {
                // 不为空，说明有任务的VD比这个任务VS大
                // 那就抢占

                // 处理抢占的任务，将它归入所对应优先级的队列之中，并从运行队列中删除
                SchedulerHelper::running_queue.remove( running_pcb.general_task_node );
                SchedulerHelper::task_queue[ running_pcb.priority ].append( running_pcb.general_task_node );
                running_pcb.flags.state = PCB::State::READY;
                // 处理抢占者，从任务队列中剔除，加入运行队列
                SchedulerHelper::task_queue[ pcb->priority ].append( pcb->general_task_node );
                SchedulerHelper::running_queue.append( pcb->general_task_node );
                pcb->cpu_id      = running_pcb.cpu_id;
                pcb->flags.state = PCB::State::RUNNING;

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
        SchedulerHelper::task_queue[ pcb->priority ].append( pcb->general_task_node );
        pcb->flags.state = PCB::State::READY;
        SchedulerHelper::global_lock.release( );
        Interrupt::enable_interrupt( );
        return pcb;
    }
    // 任务睡眠
    auto BrainFuckScheduler::__sleep__( IN uint64_t ticks ) -> std::expected< PCB *, ErrorCode > {
    }

    auto BrainFuckScheduler::__schedule__( VOID ) -> std::expected< PCB *, ErrorCode > {
        for ( auto cpu_id = Apic::apic_id( ); auto &pcb : SchedulerHelper::running_queue ) {
            if ( cpu_id == pcb.cpu_id ) {
                if ( pcb.jiffies ) {
                    pcb.jiffies--;
                    return &pcb;
                }
                else {
                    // 时间片耗尽
                    auto result = this->__pick_next__( );

                    while ( true ) {
                    }
                    if ( result.has_value( ) ) {
                        auto new_pcb = result.value( );
                        SchedulerHelper::task_queue[ new_pcb->priority ].remove( new_pcb->general_task_node );

                        SchedulerHelper::running_queue.remove( pcb.general_task_node );
                        SchedulerHelper::task_queue[ pcb.priority ].append( pcb.general_task_node );
                        pcb.flags.state = PCB::State::READY;
                        // 重新计算VD，填充时间片
                        pcb.virtual_deadline = SchedulerHelper::make_virtual_deadline( pcb.priority );
                        pcb.jiffies          = SchedulerHelper::rr_interval;

                        new_pcb->flags.state = PCB::State::RUNNING;
                        new_pcb->cpu_id      = pcb.cpu_id;

                        SchedulerHelper::running_queue.append( new_pcb->general_task_node );
                        return new_pcb;
                    }
                }
            }
        }
        return std::unexpected { ErrorCode::NO_TASK_CAN_SCHEDULER };
    }
}