#include <Kernel/task.hpp>
#include <Lib/STL/string>
#include <Arch/Arch.hpp>
#include <Lib/IO/Stream/iostream>

PUBLIC namespace {
    using namespace QuantumNEC;
    using namespace QuantumNEC::Kernel;

    TaskLock lock { };
    /**
     * @brief 进程起始函数（类似于_start）
     */
}
PUBLIC namespace QuantumNEC::Kernel {
    // Process::Process( VOID ) noexcept {
    //     Lib::STL::list_init( &this->all_task_queue );
    //     Lib::STL::list_init( &this->ready_task_queue );
    //     // 准备内核进程PCB, IDLE PCB
    //     this->main_task = reinterpret_cast< decltype( this->main_task ) >( get_current< ProcessPCB >( ) );
    //     Lib::STL::memset( this->main_task, 0, sizeof( ProcessPCB ) );
    //     this->main_task->all_task_queue.container = this->main_task;
    //     this->main_task->general_task_queue.container = this->main_task;
    //     Lib::STL::list_add_to_end( &this->all_task_queue, &this->main_task->all_task_queue );
    //     this->main_task->page_directory = NULL;
    //     this->main_task->stack = reinterpret_cast< decltype( this->main_task->stack ) >( this->main_task + 1 );
    //     this->main_task->cpu_frame = reinterpret_cast< Lib::Types::Ptr< ProcessFrame > >( this->main_task + 1 );
    //     this->main_task->fpu_frame = reinterpret_cast< Lib::Types::Ptr< Architecture::ArchitectureManager< TARGET_ARCH >::FPUFrame > >( this->main_task->cpu_frame + 1 );
    //     this->main_task->PID = this->pid_pool.allocate( );
    //     Lib::STL::strcpy( this->main_task->name, "SYSTEM" );
    //     this->main_task->message.send_to = NO_TASK;
    //     this->main_task->message.receive_from = NO_TASK;
    //     Lib::STL::list_init( &this->main_task->message.sender_list );
    //     this->main_task->status = TaskStatus::RUNNING;
    //     this->main_task->type = TaskType::PF_KERNEL_PROCESS;
    //     this->main_task->flags = static_cast< Lib::Types::int64_t >( TaskFlags::FPU_USED );
    //     this->main_task->counter = 0;
    //     this->main_task->priority = 31;
    //     this->main_task->ticks = this->main_task->priority;
    //     this->main_task->signal = 0;
    //     this->main_task->stack_magic = TASK_STACK_MAGIC;
    //     this->idle_task = this->create( idle, 0, TaskType::PF_KERNEL_PROCESS, "IDLE", 31, static_cast< Lib::Types::int64_t >( TaskFlags::FPU_UNUSED ) );
    // }
    // Process::~Process( VOID ) noexcept {
    // }
    // auto Process::create( IN TaskFunction entry, IN CONST TaskArg arg, IN CONST TaskType type, IN IN CONST Lib::Types::Ptr< CONST Lib::Types::char_t > name, IN Lib::Types::uint64_t priority, IN CONST Lib::Types::int64_t flags )->Lib::Types::Ptr< ProcessPCB > {
    //     lock.acquire( );
    //     Lib::Types::Ptr< ProcessPCB > task { reinterpret_cast< decltype( task ) >( PageMemory::malloc( 1, PageMemory::MemoryPageType::PAGE_2M ) ) };
    //     if ( !task ) {
    //         return NULL;
    //     }
    //     // 将任务pcb添加到队列
    //     task->all_task_queue.container = task;
    //     task->general_task_queue.container = task;
    //     Lib::STL::list_add_to_end( &ready_task_queue, &task->general_task_queue );
    //     Lib::STL::list_add_to_end( &all_task_queue, &task->all_task_queue );
    //     // 制作页表
    //     task->page_directory = ( type == TaskType::PF_KERNEL_PROCESS ? NULL : reinterpret_cast< decltype( task->page_directory ) >( MemoryMap::make_page_table( ) ) );
    //     if ( !task->page_directory && type == TaskType::PF_USER_PROCESS ) {
    //         Lib::STL::memset( task, 0, TASK_STACK_SIZE );
    //         PageMemory::free( task, 1, PageMemory::MemoryPageType::PAGE_2M );
    //         return NULL;
    //     }
    //     task->stack = reinterpret_cast< decltype( task->stack ) >( reinterpret_cast< Lib::Types::uint64_t >( task ) + sizeof( ProcessPCB ) );     // 分配栈空间（2M - 64KB）
    //     task->allocate_table = Lib::Base::Allocate { sizeof( Lib::Base::Allocate ::AllocateTableEntry ) * 1024 };
    //     task->allocate_table.free( USER_STACK_VIRTUAL_START_ADDRESS / PAGE_SIZE, ( USER_STACK_VIRTUAL_END_ADDRESS - USER_STACK_VIRTUAL_START_ADDRESS ) / PAGE_SIZE );

    //     /*
    //      * 分布如下
    //      * ---------------------------------------64KB-----------------------------------------
    //      * sizeof(TaskPCB)---cpu_frame---fpu_frame---进程栈 ( RSP 指向64KB尾巴 )
    //      * Task PCB        ~ 存CPU状态  ~ FPU状态    ~ 空                  ~ TOP
    //      */
    //     // 分配 cpu frame
    //     task->cpu_frame = reinterpret_cast< Lib::Types::Ptr< ProcessFrame > >( task + 1 );
    //     if ( !do_fork( task->cpu_frame, entry, arg, type ) ) {
    //         // 如果分配失败那么就释放所有的内存并返回
    //         Lib::STL::memset( task->cpu_frame, 0, sizeof( ProcessFrame ) );
    //         Lib::STL::memset( task, 0, TASK_STACK_SIZE );
    //         PageMemory::free( task, 1, PageMemory::MemoryPageType::PAGE_2M );
    //         return NULL;
    //     }
    //     // 分配 fpu frame
    //     task->fpu_frame = reinterpret_cast< Lib::Types::Ptr< Architecture::ArchitectureManager< TARGET_ARCH >::FPUFrame > >( task->cpu_frame + 1 );
    //     if ( auto current_fpu { Process::get_current< ProcessPCB >( )->fpu_frame }; current_fpu ) {
    //         Lib::STL::memcpy( task->fpu_frame, current_fpu, sizeof *current_fpu );
    //     }
    //     task->message.send_to = NO_TASK;
    //     task->message.receive_from = NO_TASK;
    //     Lib::STL::list_init( &task->message.sender_list );
    //     // 分配PID
    //     task->PID = pid_pool.allocate( );
    //     // 设置进程名
    //     Lib::STL::strncpy( task->name, name, TASK_NAME_SIZE );
    //     // 设置进程初始状态
    //     task->status = TaskStatus::READY;
    //     // 进程类型
    //     task->type = type;
    //     // 计算这个进程使用的总时间片
    //     task->counter = 0;
    //     // 这个进程可的使用时间
    //     task->ticks = priority;
    //     // 暂时没啥用
    //     task->signal = 0;
    //     // 时间片越多优先级越高
    //     task->priority = priority;
    //     task->flags = flags;
    //     task->stack_magic = TASK_STACK_MAGIC;
    //     lock.release( );
    //     return task;
    // }
    auto ProcessFrame::make( IN TaskFunction entry, IN CONST TaskArg arg, IN Lib::Types::int64_t _flags )->Lib::Types::BOOL {
        lock.acquire( );

        if ( _flags & TASK_FLAG_KERNEL_PROCESS ) {
            // 内核进程的
            this->regs.ds = Architecture::SELECTOR_DATA64_KERNEL;
            this->regs.es = Architecture::SELECTOR_DATA64_KERNEL;
            this->regs.fs = Architecture::SELECTOR_DATA64_KERNEL;
            this->regs.gs = Architecture::SELECTOR_DATA64_KERNEL;
            this->cs = Architecture::SELECTOR_CODE64_KERNEL;
            this->ss = Architecture::SELECTOR_DATA64_KERNEL;
            this->rflags = Architecture::RFLAGS_IOPL_3 | Architecture::RFLAGS_MBS | Architecture::RFLAGS_IF_1;
        }
        else if ( _flags & TASK_FLAG_USER_PROCESS ) {
            // 用户进程的
            this->regs.ds = Architecture::SELECTOR_DATA64_USER;
            this->regs.es = Architecture::SELECTOR_DATA64_USER;
            this->regs.fs = Architecture::SELECTOR_DATA64_USER;
            this->regs.gs = Architecture::SELECTOR_DATA64_USER;
            this->cs = Architecture::SELECTOR_CODE64_USER;
            this->ss = Architecture::SELECTOR_DATA64_USER;
            this->rflags = Architecture::RFLAGS_IOPL_0 | Architecture::RFLAGS_MBS | Architecture::RFLAGS_IF_1;
        }
        else {     // 分配失败
            lock.release( );
            return FALSE;
        }
        this->regs.rax = 0;
        this->regs.rbx = 0;
        this->regs.rcx = 0;
        this->regs.rdx = 0;
        this->regs.rbp = 0;
        this->regs.rsi = 0;
        this->regs.rdi = reinterpret_cast< decltype( this->regs.rdi ) >( arg );
        this->regs.r8 = 0;
        this->regs.r9 = 0;
        this->regs.r10 = 0;
        this->regs.r11 = 0;
        this->regs.r12 = 0;
        this->regs.r13 = 0;
        this->regs.r14 = 0;
        this->regs.r15 = 0;
        this->regs.rbp = 0;
        // rip存进程入口
        this->rip = reinterpret_cast< decltype( this->rip ) >( entry );
        this->rsp = reinterpret_cast< decltype( this->rsp ) >( this ) - sizeof( Process ) + TASK_STACK_SIZE;
        lock.release( );
        return TRUE;
    }
    auto Process::schedule( VOID )->VOID {
        auto current { get_current< Process >( ) };
        if ( ( current->all_task_queue.container != current || current->general_task_queue.container != current || current->stack_magic != TASK_STACK_MAGIC ) ) {
            Lib::IO::sout[ Lib::IO::ostream::HeadLevel::WARNING ] << "Task allocate error." << '\n';
            while ( TRUE )
                ;
        }

        if ( current->status == TaskStatus::RUNNING ) {
            Lib::STL::list_add_to_end( &ready_task_group, &current->general_task_queue );
            current->ticks = current->priority;
            current->status = TaskStatus::READY;
        }
        if ( Lib::STL::list_is_empty( &ready_task_group ) ) {
            this->unblock( Task::idle_task );
        }
        Lib::Types::Ptr< Lib::STL::ListNode > next_task_queue { Lib::STL::list_pop( &ready_task_group ) };
        auto next { reinterpret_cast< Process * >( next_task_queue->container ) };
        next->status = TaskStatus::RUNNING;
        if ( this->flags & TASK_FLAG_FPU_UNUSED ) {
            this->fpu_frame->disable_fpu( current );     // 关闭FPU
        }
        this->activate( next );     // 激活页表
        Task::ready_task = next;
        return;
    }
    auto Process::block( IN TaskStatus state )->VOID {
        auto interrupt_status { Architecture::ArchitectureManager< TARGET_ARCH >::disable_interrupt( ) };
        this->status = state;
        this->schedule( );
        Architecture::ArchitectureManager< TARGET_ARCH >::set_interrupt( interrupt_status );
        return;
    }
    auto Process::unblock( IN Lib::Types::Ptr< Process > pcb )->VOID {
        if ( !pcb ) {
            return;
        }
        auto interrupt_status { Architecture::ArchitectureManager< TARGET_ARCH >::disable_interrupt( ) };
        if ( pcb->status != TaskStatus::READY ) {
            // 放到队列的最前面，使其尽快得到调度
            Lib::STL::list_add_to_head( &ready_task_group, &pcb->general_task_queue );
            pcb->status = TaskStatus::READY;
        };
        Architecture::ArchitectureManager< TARGET_ARCH >::set_interrupt( interrupt_status );
        return;
    }
}
