// #include <Kernel/task/task.hpp>
// #include <Arch/Arch.hpp>
// #include <Kernel/print.hpp>
// #include <Libcxx/string.hpp>
// #include <Lib/spin_lock.hpp>
// PUBLIC namespace {
//     using namespace QuantumNEC::Kernel;
//     using namespace QuantumNEC::Lib;

//     PRIVATE SpinLock lock { };
// }
// VOID *__frame;
// PUBLIC namespace QuantumNEC::Kernel {
//     auto ThreadFrame::make( IN TaskFunction _entry, IN TaskArg _arg, IN int64_t _flags )->BOOL {
//         if ( _flags & TASK_FLAG_THREAD ) {
// #if defined( __x86_64__ )
//             // 没啥好解释的。。。
//             this->rdi = _arg;
//             this->rsi = 0;
//             this->rip = reinterpret_cast< decltype( this->rip ) >( _entry );
//             this->rbp = 0;
//             this->rbx = 0;
//             this->r15 = 0;
//             this->r14 = 0;
//             this->r13 = 0;
//             this->r12 = 0;
//             this->rflags.IOPL = 0;
//             this->rflags.MBS = 1;
//             this->rflags.IF = 1;
// #elif defined( __aarch64__ )
// #else
// #error There are no instructions available to set the registers
// #endif
//             return TRUE;
//         }
//         else {
//             return FALSE;
//         }
//     }

//     auto Thread::schedule( VOID )->VOID {
//         if ( ( this->all_task_queue.container != this || this->general_task_queue.container != this || this->stack_magic != TASK_STACK_MAGIC ) ) {
//             while ( TRUE )
//                 ;
//         }
//         if ( this->status == TaskStatus::RUNNING ) {
//             ready_task_group.append( this->general_task_queue );
//             this->ticks = this->priority;
//             this->status = TaskStatus::READY;
//         }
//         if ( ready_task_group.is_empty( ) ) {
//             Task::idle_task->unblock( );
//         }
//         auto next_task_queue { ready_task_group.pop( ) };
//         auto next { reinterpret_cast< Thread * >( next_task_queue->container ) };
//         next->status = TaskStatus::RUNNING;

//         if ( this->flags & TASK_FLAG_FPU_UNUSED ) {
//             this->fpu->disable_fpu( this );     // 关闭FPU
//         }

//         if ( next->flags & TASK_FLAG_THREAD ) {
//             if ( this->flags & TASK_FLAG_THREAD ) {
//                 Architecture::switch_to(
//                     &this->context,
//                     &next->context );
//             }
//         }
//         else {
//             Architecture::switch_to( &this->context, &( (ProcessFrame *)next->context )->tframe );
//         }
//     }
//     auto Thread::block( IN TaskStatus state )->VOID {
//         auto interrupt_status { Architecture::ArchitectureManager< TARGET_ARCH >::disable_interrupt( ) };
//         this->status = state;
//         this->schedule( );
//         Architecture::ArchitectureManager< TARGET_ARCH >::set_interrupt( interrupt_status );
//         return;
//     }
//     auto Thread::unblock( VOID )->VOID {
//         auto interrupt_status { Architecture::ArchitectureManager< TARGET_ARCH >::disable_interrupt( ) };
//         if ( this->status != TaskStatus::READY ) {
//             // 放到队列的最前面，使其尽快得到调度
//             ready_task_group.push( this->general_task_queue );
//             this->status = TaskStatus::READY;
//         };
//         Architecture::ArchitectureManager< TARGET_ARCH >::set_interrupt( interrupt_status );
//         return;
//     }
// }
