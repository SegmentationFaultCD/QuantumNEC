// // #include <Kernel/task/task.hpp>
// // #include <Libcxx/string.hpp>
// // #include <Arch/Arch.hpp>
// // #include <Kernel/print.hpp>
// // #include <Lib/spin_lock.hpp>
// // PUBLIC namespace {
// //     using namespace QuantumNEC;
// //     using namespace QuantumNEC::Kernel;
// //     using namespace QuantumNEC::Lib;

// //     PRIVATE SpinLock lock { };
// // }
// // PUBLIC namespace QuantumNEC::Kernel {
// // auto ProcessFrame::make( IN TaskFunction entry, IN CONST TaskArg arg, IN int64_t _flags ) -> BOOL {
// //     lock.acquire( );

// //     if ( _flags & TASK_FLAG_KERNEL_PROCESS ) {
// //         // 内核进程的
// //         this->iframe.regs.ds = Architecture::SELECTOR_DATA64_KERNEL;
// //         this->iframe.regs.es = Architecture::SELECTOR_DATA64_KERNEL;
// //         this->iframe.regs.fs = Architecture::SELECTOR_DATA64_KERNEL;
// //         this->iframe.regs.gs = Architecture::SELECTOR_DATA64_KERNEL;
// //         this->iframe.cs = Architecture::SELECTOR_CODE64_KERNEL;
// //         this->iframe.ss = Architecture::SELECTOR_DATA64_KERNEL;
// //         this->iframe.rflags.IOPL = 0;
// //         this->iframe.rflags.MBS = 1;
// //         this->iframe.rflags.IF = 1;
// //     }
// //     else if ( _flags & TASK_FLAG_USER_PROCESS ) {
// //         // 用户进程的
// //         this->iframe.regs.ds = Architecture::SELECTOR_DATA64_USER;
// //         this->iframe.regs.es = Architecture::SELECTOR_DATA64_USER;
// //         this->iframe.regs.fs = Architecture::SELECTOR_DATA64_USER;
// //         this->iframe.regs.gs = Architecture::SELECTOR_DATA64_USER;
// //         this->iframe.cs = Architecture::SELECTOR_CODE64_USER;
// //         this->iframe.ss = Architecture::SELECTOR_DATA64_USER;
// //         this->iframe.rflags.IOPL = 3;
// //         this->iframe.rflags.MBS = 1;
// //         this->iframe.rflags.IF = 1;
// //     }
// //     else {     // 分配失败
// //         lock.release( );
// //         return FALSE;
// //     }
// //     this->iframe.regs.rax = 0;
// //     this->iframe.regs.rbx = 0;
// //     this->iframe.regs.rcx = 0;
// //     this->iframe.regs.rdx = 0;
// //     this->iframe.regs.rbp = 0;
// //     this->iframe.regs.rsi = 0;
// //     this->iframe.regs.rdi = reinterpret_cast< decltype( this->iframe.regs.rdi ) >( arg );
// //     this->iframe.regs.r8 = 0;
// //     this->iframe.regs.r9 = 0;
// //     this->iframe.regs.r10 = 0;
// //     this->iframe.regs.r11 = 0;
// //     this->iframe.regs.r12 = 0;
// //     this->iframe.regs.r13 = 0;
// //     this->iframe.regs.r14 = 0;
// //     this->iframe.regs.r15 = 0;
// //     this->iframe.regs.rbp = 0;
// //     // rip存进程入口
// //     this->iframe.rip = reinterpret_cast< decltype( this->iframe.rip ) >( entry );
// //     // 进程自己本身持有一个线程
// //     // 所以必须有一个线程栈
// //     // 所以在任务栈末尾先放线程栈，再放进程栈
// //     this->iframe.rsp = reinterpret_cast< decltype( this->iframe.rsp ) >( this - 1 );
// //     lock.release( );
// //     return TRUE;
// // }
// // auto Process::schedule( VOID ) -> VOID {
// //     if ( ( this->all_task_queue.container != this || this->general_task_queue.container != this || this->stack_magic != TASK_STACK_MAGIC ) ) {
// //         while ( TRUE );
// //     }
// //     if ( this->status == TaskStatus::RUNNING ) {
// //         ready_task_group.append( this->general_task_queue );
// //         this->ticks = this->priority;
// //         this->status = TaskStatus::READY;
// //     }
// //     if ( ready_task_group.is_empty( ) ) {
// //         Task::idle_task->unblock( );
// //     }

// //     auto next_task_queue { ready_task_group.pop( ) };
// //     auto next { reinterpret_cast< Process * >( next_task_queue->container ) };
// //     next->status = TaskStatus::RUNNING;
// //     if ( this->flags & TASK_FLAG_FPU_UNUSED ) {
// //         this->fpu->disable_fpu( this );     // 关闭FPU
// //     }
// //     if ( next->flags & TASK_FLAG_THREAD ) {
// //         Architecture::switch_to( &( (ProcessFrame *)this->context )->tframe, &next->context );
// //     }
// //     else {
// //         next->activate( );     // 激活页表
// //         Task::ready_task = next;
// //     }
// //     return;
// // }
// // auto Process::block( IN TaskStatus state ) -> VOID {
// //     auto interrupt_status { Architecture::ArchitectureManager< TARGET_ARCH >::disable_interrupt( ) };
// //     this->status = state;
// //     this->schedule( );
// //     this->ticks = 0;
// //     Architecture::ArchitectureManager< TARGET_ARCH >::set_interrupt( interrupt_status );
// //     return;
// // }
// // auto Process::unblock( VOID ) -> VOID {
// //     auto interrupt_status { Architecture::ArchitectureManager< TARGET_ARCH >::disable_interrupt( ) };
// //     if ( this->status != TaskStatus::READY ) {
// //         // 放到队列的最前面，使其尽快得到调度
// //         ready_task_group.push( this->general_task_queue );
// //         this->status = TaskStatus::READY;
// //     };
// //     Architecture::ArchitectureManager< TARGET_ARCH >::set_interrupt( interrupt_status );
// //     return;
// // }
// }
