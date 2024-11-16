#include <kernel/task/general/scheduler/bfs.hpp>
PUBLIC namespace QuantumNEC::Kernel {
    auto BrainFuckScheduler::__pick_next__( VOID ) -> std::expected< PCB *, ErrorCode > {
        // auto index = 0;
        // for ( ; index < 103; ++index ) {
        //     if ( this->bitmap[ index ] ) {
        //         break;
        //     }
        // }
        // if ( index < 100 ) {
        //     // 实时任务，直接弹出首位
        //     auto pcb = (PCB *)this->task_queue[ index ].begin( )->container;
        //     this->global_lock.release( );
        //     return pcb;
        // }
        // else {
        //     // EEVDF算法
        //     for ( auto i = index; i < 103; ++i ) {
        //         if ( this->task_queue[ i ].is_empty( ) ) {
        //             continue;
        //         }
        //         auto head = this->task_queue[ i ].begin( );
        //         this->global_lock.acquire( );
        //         Lib::ListNode *p, *q, *tail;                                // 创建三个指针，进行冒泡排序
        //         auto           count = this->task_queue[ i ].length( );     // 记录链表结点的个数
        //         for ( auto i = 0ul; i < count - 1; i++ )                    // 外层循环，跟数组冒泡排序一样
        //         {
        //             auto num = count - i - 1;     // 记录内层循环需要的次数，跟数组冒泡排序一样，
        //             q        = head;              // 令q指向第一个结点
        //             p        = q->next;           // 令p指向第二个结点
        //             tail     = head->prev;        // 让tail始终指向q前一个结点，方便交换，也方便与进行下一步操作
        //             while ( num-- )               // 内层循环,次数跟数组冒泡排序一样
        //             {
        //                 if ( auto pcb = ( (PCB *)q->container ); pcb->virtual_deadline < Interrupt::global_jiffies ) {
        //                     // deadline小于当前进程则直接弹出
        //                     this->global_lock.release( );
        //                     return pcb;
        //                 }
        //                 if ( ( (PCB *)q->container )->virtual_deadline > ( (PCB *)p->container )->virtual_deadline )     // 如果该结点的值大于后一个结点，则交换
        //                 {
        //                     q->next    = p->next;
        //                     p->next    = q;
        //                     tail->next = p;
        //                 }
        //                 // 进行指针的移动
        //                 tail = tail->next;
        //                 q    = tail->next;
        //                 p    = q->next;
        //             }
        //         }
        //         this->global_lock.release( );
        //         return (PCB *)head->container;
        //     }
        // }
        // return std::unexpected { ErrorCode::ALL_QUEUE_ARE_EMPTY };
    }
    // 任务唤醒
    auto BrainFuckScheduler::__insert__( PCB * pcb ) -> std::expected< PCB *, ErrorCode > {
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
        // 不为空，说明有任务的VD比这个任务VS大
        // 那就抢占
        else {
            // 处理被抢占的任务，将它归入所对应优先级的队列之中，并从运行队列中删除
            auto can_seize_pcb = (PCB *)node->container;
            this->running_queue.remove( can_seize_pcb->general_task_node );
            this->task_queue[ can_seize_pcb->priority ].append( can_seize_pcb->general_task_node );
            // 处理抢占者，从任务队列中剔除，加入运行队列
            this->task_queue[ pcb->priority ].append( pcb->general_task_node );
            this->running_queue.append( pcb->general_task_node );
            pcb->cpu_id      = can_seize_pcb->cpu_id;
            pcb->flags.state = PCB::State::RUNNING;
            // 并计算虚拟截止时间
            pcb->virtual_deadline = Interrupt::global_jiffies + this->prio_ratios[ pcb->priority ] * this->rr_interval;
            this->global_lock.release( );

            // 发送IPI呼唤对应CPU删除正在运行的任务
        }
    }

    auto BrainFuckScheduler::__wake_up__( PCB * pcb ) -> std::expected< PCB *, ErrorCode > {
        //         auto current = get_current( );

        //         if ( node ) {     // 不为空说明可以抢占
        //             auto can_seize_pcb = (PCB *)node->container;
        //             if ( can_seize_pcb == current ) {
        //                 this->running_queue.remove( can_seize_pcb->general_task_node );
        //                 this->task_queue[ can_seize_pcb->priority ].append( can_seize_pcb->general_task_node );
        //                 pcb->cpu_id      = can_seize_pcb->cpu_id;
        //                 pcb->flags.state = PCB::State::RUNNING;
        //                 this->running_queue.append( pcb->general_task_node );
        //                 pcb->virtual_deadline = Interrupt::global_jiffies + this->prio_ratios[ pcb->priority ] * this->rr_interval;
        //             }
        //             /*
        // ICR_DELIVER_MODE_FIXED,
        //     ICR_DEST_MODE_PHY,
        //     ICR_DELIVER_STATUS_IDLE,
        //     ICR_LEVEL_DE_ASSEST,
        //     ICR_TRIGGER_EDGE,
        //     ICR_ALL_EXCLUDE_SELF,
        // */
        //         }
        //         this->task_queue[ pcb->priority ].append( pcb->general_task_node );
        //         return current;
    }
    // 任务睡眠
    auto BrainFuckScheduler::__sleep__( VOID ) -> std::expected< PCB *, ErrorCode > {
        // auto current = get_current( );
        // this->task_queue[ current->priority ].remove( current->general_task_node );
        // auto result = this->pick_next( );
        // if ( result.has_value( ) ) {
        //     return result.value( );
        // }
        // return result;
    }
    auto BrainFuckScheduler::__schedule__( VOID ) -> std::expected< PCB *, ErrorCode > {
        return (PCB *)NULL;
    }
    //     auto current = get_current( );
    //     if ( !current->jiffies ) {     // 时间片耗尽
    //         current->jiffies = rr_interval;
    //         current->virtual_deadline = Architecture::ArchitectureManager< TARGET_ARCH >::global_jiffies + prio_ratios[ current->priority ] * rr_interval;
    //         const auto then = this->insert( current );
    //         if ( !then.has_value( ) ) {
    //             if ( then.error( ) == ErrorCode::ALL_QUEUE_ARE_EMPTY ) {
    //                 // TODO
    //                 // 错误处理
    //             }
    //             return current;
    //         }
    //         return then;
    //     }
    //     else {
    //         current->jiffies--;
    //         return current;
    //     }
    // }
}