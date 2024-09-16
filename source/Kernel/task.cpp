#include <Kernel/task.hpp>
#include <Lib/IO/Stream/iostream>
PUBLIC namespace QuantumNEC::Kernel {
    PRIVATE auto idle( IN TaskArg )->TaskReturnValue {
        while ( TRUE ) {
            get_current< Process >( )->block( TaskStatus::BLOCKED );
            Architecture::ArchitectureManager< TARGET_ARCH >::sti( );
            Architecture::ArchitectureManager< TARGET_ARCH >::hlt( );
        }
        return 0;
    }
    Task::Task( IN Lib::Types::Ptr< Lib::Types::BootConfig > ) noexcept {
        Lib::STL::list_init( &all_task_group );
        Lib::STL::list_init( &ready_task_group );
        // 准备内核进程PCB, IDLE PCB
        this->main_task = get_current< Process >( );
        Lib::STL::memset( this->main_task, 0, sizeof( Process ) );
        this->main_task->all_task_queue.container = this->main_task;
        this->main_task->general_task_queue.container = this->main_task;
        Lib::STL::list_add_to_end( &all_task_group, &this->main_task->all_task_queue );
        this->main_task->page_directory = NULL;
        this->main_task->cpu_frame = reinterpret_cast< Lib::Types::Ptr< ProcessFrame > >( this->main_task + 1 );
        this->main_task->fpu_frame = reinterpret_cast< Lib::Types::Ptr< Architecture::ArchitectureManager< TARGET_ARCH >::FPUFrame > >( this->main_task->cpu_frame + 1 );
        this->main_task->PID = PidPool::allocate( );
        Lib::STL::strcpy( this->main_task->name, "SYSTEM" );
        this->main_task->message.send_to = NO_TASK;
        this->main_task->message.receive_from = NO_TASK;
        Lib::STL::list_init( &this->main_task->message.sender_list );
        this->main_task->status = TaskStatus::RUNNING;
        this->main_task->flags = TASK_FLAG_FPU_USED | TASK_FLAG_KERNEL_PROCESS;
        this->main_task->counter = 0;
        this->main_task->priority = 31;
        this->main_task->ticks = this->main_task->priority;
        this->main_task->signal = 0;
        this->main_task->stack_magic = TASK_STACK_MAGIC;
        this->idle_task = new ( Memory::page->malloc( 1, PageMemory::MemoryPageType::PAGE_2M ) ) Process { "IDLE", 31, TASK_FLAG_FPU_UNUSED | TASK_FLAG_KERNEL_PROCESS, idle, 0 };
    }
    Task::~Task( VOID ) noexcept {
    }
}