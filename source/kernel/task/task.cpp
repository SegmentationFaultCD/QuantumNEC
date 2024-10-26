// #include <Kernel/task/task.hpp>
// #include <Kernel/print.hpp>
// #include <Lib/spin_lock.hpp>
// PUBLIC namespace QuantumNEC::Kernel {
//     // using namespace Lib;
// // PRIVATE SpinLock lock { };

// // PRIVATE auto idle( IN TaskArg )->TaskReturnValue {
// //     while ( TRUE ) {
// //         get_current< Process >( )->block( TaskStatus::BLOCKED );
// //         Architecture::ArchitectureManager< TARGET_ARCH >::sti( );
// //         Architecture::ArchitectureManager< TARGET_ARCH >::hlt( );
// //     }
// //     return 0;
// // }

// // Task::Task( VOID ) noexcept {
// //     // 准备内核进程PCB, IDLE PCB

// //     this->main_task = get_current< Process >( );
// //     memset( this->main_task, 0, sizeof( Process ) );
// //     this->main_task->all_task_queue.container = this->main_task;
// //     this->main_task->general_task_queue.container = this->main_task;
// //     all_task_group.append( this->main_task->all_task_queue );

// //     this->main_task->context = reinterpret_cast< ProcessFrame * >( uint64_t( this->main_task ) + TASK_STACK_SIZE ) - 1;

// //     this->main_task->context->tframe = (ThreadFrame *)this->main_task->context - 1;

// //     this->main_task->fpu = reinterpret_cast< Architecture::ArchitectureManager< TARGET_ARCH >::FPUFrame * >( this->main_task + 1 );

// //     this->main_task->PID = pid_pool.allocate( );
// //     strcpy( this->main_task->name, "SYSTEM" );
// //     this->main_task->message.send_to = Architecture::NO_TASK;
// //     this->main_task->message.receive_from = Architecture::NO_TASK;

// //     this->main_task->status = TaskStatus::RUNNING;
// //     this->main_task->flags = TASK_FLAG_FPU_USED | TASK_FLAG_KERNEL_PROCESS;
// //     this->main_task->priority = 31;
// //     this->main_task->ticks = this->main_task->priority;
// //     this->main_task->signal = 0;
// //     this->main_task->stack_magic = TASK_STACK_MAGIC;

// //     this->idle_task = this->create< Process >( "IDLE", 31, TASK_FLAG_FPU_UNUSED | TASK_FLAG_KERNEL_PROCESS, idle, 0 );

// //     this->ready_task = this->main_task;
// // }
// // Task::~Task( VOID ) noexcept {
// // }
// }
