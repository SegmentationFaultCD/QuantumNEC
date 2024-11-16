#include <kernel/task/general/scheduler/bfs.hpp>
PUBLIC namespace QuantumNEC::Kernel {
    auto BrainFuckScheduler::__pick_next__( VOID ) -> std::expected< PCB *, ErrorCode > {
        // 针对不同优先级采用的pick next方法
        auto index = 0;
        for ( ; index < this->total_priority; ++index ) {
            // 遍历队列查看是否有空
            if ( this->bitmap[ index ] ) {
                break;
            }
        }
        if ( index < 100 ) {
            // 实时任务，直接弹出首位
            auto pcb = (PCB *)this->task_queue[ index ].begin( )->container;
            this->global_lock.release( );
            return pcb;
        }
        else {
            for ( auto i = index; i < this->total_priority; ++i ) {
                if ( this->task_queue[ i ].is_empty( ) ) {
                    continue;
                }
                auto head = this->task_queue[ i ].begin( );
                this->global_lock.acquire( );
                // EEVDF算法在剩余三个队列中整理并查找
                Lib::ListNode *p, *q, *tail;
                auto           count = this->task_queue[ i ].length( );
                for ( auto i = 0ul; i < count - 1; i++ ) {
                    auto num = count - i - 1;
                    q        = head;
                    p        = q->next;
                    tail     = head->prev;
                    while ( num-- ) {
                        if ( auto pcb = ( (PCB *)q->container ); pcb->virtual_deadline < Interrupt::global_jiffies ) {
                            // deadline小于当前进程则直接弹出这个
                            this->global_lock.release( );
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
                this->global_lock.release( );
                // 全部整理一遍后，弹出头
                return (PCB *)head->container;
            }
        }
        // 查找失败
        return std::unexpected { ErrorCode::ALL_QUEUE_ARE_EMPTY };
    }

    auto BrainFuckScheduler::__wake_up__( PCB * pcb ) -> std::expected< PCB *, ErrorCode > {
        this->global_lock.acquire( );
        // 遍历运行队列查找是否可以抢占VD大于插入任务的CPU
        auto node = this->running_queue.traversal(
            []( Lib::ListNode *node, uint64_t virtual_deadline ) static -> BOOL {
                return ( (PCB *)node->container )->virtual_deadline > virtual_deadline;
            },
            pcb->virtual_deadline );
        if ( !node ) {
            // 为空，说明这个要插入的任务VD太大了
            // 那就根据优先级插入任务等待队列
            // 插入队尾
            this->task_queue[ pcb->priority ].append( pcb->general_task_node );
            pcb->flags.state = PCB::State::READY;
            this->global_lock.release( );
            return pcb;
        }
        else {
            // 不为空，说明有任务的VD比这个任务VS大
            // 那就抢占

            auto can_seize_pcb = (PCB *)node->container;
            // 处理抢占的任务，将它归入所对应优先级的队列之中，并从运行队列中删除
            this->running_queue.remove( can_seize_pcb->general_task_node );
            this->task_queue[ can_seize_pcb->priority ].append( can_seize_pcb->general_task_node );
            can_seize_pcb->flags.state = PCB::State::READY;
            // 处理抢占者，从任务队列中剔除，加入运行队列
            this->task_queue[ pcb->priority ].append( pcb->general_task_node );
            this->running_queue.append( pcb->general_task_node );
            pcb->cpu_id      = can_seize_pcb->cpu_id;
            pcb->flags.state = PCB::State::RUNNING;

            this->global_lock.release( );
            CPU::switch_cpu( );     // 切换CPU，在切换后进行换值
            return pcb;
        }
    }
    // 任务睡眠
    auto BrainFuckScheduler::__sleep__( IN uint64_t ticks ) -> std::expected< PCB *, ErrorCode > {
    }

    auto BrainFuckScheduler::__schedule__( VOID ) -> std::expected< PCB *, ErrorCode > {
        auto pcb = (PCB *)this->running_queue.traversal(
                                                 []( Lib::ListNode *node, uint64_t cpu_id ) static -> BOOL {
                                                     return ( (PCB *)node->container )->cpu_id > cpu_id;
                                                 },
                                                 Apic::apic_id( ) )
                       ->container;
        if ( pcb->jiffies ) {
            pcb->jiffies--;
            return pcb;
        }
        else {
            // 时间片耗尽
            auto result = this->__pick_next__( );
            if ( result.has_value( ) ) {
                auto new_pcb = result.value( );
                this->task_queue[ new_pcb->priority ].remove( new_pcb->general_task_node );

                this->running_queue.remove( pcb->general_task_node );
                this->task_queue[ pcb->priority ].append( pcb->general_task_node );
                pcb->flags.state = PCB::State::READY;
                // 重新计算VD，填充时间片
                pcb->virtual_deadline = Interrupt::global_jiffies + this->prio_ratios[ pcb->priority ] * this->rr_interval;
                pcb->jiffies          = this->rr_interval;

                new_pcb->flags.state = PCB::State::RUNNING;
                new_pcb->cpu_id      = pcb->cpu_id;

                this->running_queue.append( new_pcb->general_task_node );
                return new_pcb;
            }
        }
        return std::unexpected { ErrorCode::NO_TASK_CAN_SCHEDULER };
    }
}