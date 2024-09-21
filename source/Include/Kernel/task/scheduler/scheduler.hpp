#pragma once
#include <Lib/Uefi.hpp>
#include <type_traits>

PUBLIC namespace QuantumNEC::Kernel {
    template < typename SchedulerType >
        requires std::is_compound_v< SchedulerType > && requires( SchedulerType &sche ) {
            sche.wake_up( );
            sche.sleep( );
            sche.schedule( );
            sche.insert( );
        }
    class Scheduler : public SchedulerType
    {
    };
}

// #include <Kernel/task/process.hpp>
// PUBLIC namespace QuantumNEC::Kernel {
//     PUBLIC class Scheduler
//     {
//     public:
//         enum SchedulePriority {
//             // 前0～99为real time 任务
//             ISO = 100,
//             NORMAL = 101,
//             IDLEPRO = 102,
//         };

//     public:
//         explicit Scheduler( VOID ) :
//             bitmap { PRIORITY_QUEUE_COUNT, entry } {
//         }

//     public:
//         auto schedule( TaskPCB *p ) {
//             ASM( "movq %0, %%rsp\n\t"
//                  "jmp ret_from_interrupt\n\t" ::"g"( p->context.pcontext ) );
//         }
//         /**
//          * @brief 唤醒进程
//          */
//         auto insert( TaskPCB *p ) {
//             BOOL preemptible { FALSE };
//             for ( auto i { 0ul }; i < Architecture::__config.smp.cpu_count; ++i ) {
//                 // 多核，查看每个cpu正在运行的进程，如果能抢占那么就直接抢占
//             }
//             if ( !preemptible ) {     // 无法抢占
//                 // 加入相对应末尾
//                 this->priority_queue[ p->priority ].append( p->general_task_node );
//                 // 有了ready进程，设置为1
//                 this->bitmap[ p->priority ] = { p->priority, Lib::BITMAP_MASK };
//                 p->status |= static_cast< uint64_t >( TaskStatus::READY );
//             }
//             else {
//                 auto current = get_current( );
//                 // 直接抢占当前进程
//                 this->priority_queue[ current->priority ].append( current->general_task_node );
//                 this->bitmap[ current->priority ] = { current->priority, Lib::BITMAP_MASK };
//                 // 调度只可能在中断产生后调度，such as syscall, timer interrupt
//                 // 我们可以确定current的上下文已经被保存
//                 p->status |= static_cast< uint64_t >( TaskStatus::RUNNING );
//                 current->status |= static_cast< uint64_t >( TaskStatus::READY );
//                 this->schedule( p );
//             }
//         }
//         auto try_wake_up( TaskPCB *p, uint64_t status ) {
//             this->global_lock.acquire( );
//             if ( !( p->status & status ) ) {
//                 goto out_unlock;
//             }
//             if ( p->status & static_cast< uint64_t >( TaskStatus::READY ) ) {
//                 goto out_running;
//             }

//         out_running:
// #if defined( __x86_64__ )
//             Architecture::ArchitectureManager< TARGET_ARCH >::gdt->tss[ 0 ]
//                 .set_rsp0< TASK_STACK_SIZE >( p );
// #elif defined( __aarch64__ )
// #else
// #error Not any registers
// #endif
//             Memory::memory_paging_map->activate( p->page_table );
//         out_unlock:
//             this->global_lock.release( );
//         }
//         auto sleep( Process * ) {
//         }

//     private:
//         // 优先级队列，共103个，每个用来存储ready进程
//         Lib::ListTable priority_queue[ PRIORITY_QUEUE_COUNT ];
//         // 位图，当一个优先级队列中存在ready进程时为1
//         Lib::Bitmap bitmap;
//         Lib::Bitmap::BitmapEntry entry[ PRIORITY_QUEUE_COUNT ];
//         // 全局锁
//         Lib::SpinLock global_lock { };
//     };
// }