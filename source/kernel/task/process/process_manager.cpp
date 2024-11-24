#include <kernel/cpu/cpu.hpp>
#include <kernel/memory/heap/kheap/kheap_walker.hpp>
#include <kernel/memory/page/page_walker.hpp>
#include <kernel/task/process/process_creater.hpp>
#include <kernel/task/process/process_manager.hpp>
#include <kernel/task/task.hpp>
PUBLIC namespace QuantumNEC::Kernel {
    ProcessManager::ProcessManager( VOID ) noexcept {
        main_pcb = reinterpret_cast< PCB * >( KHeapWalker { }.allocate( sizeof( PCB ) ) );
        // 内核栈处理
        main_pcb->kernel_stack_base = (uint64_t)physical_to_virtual( PageWalker { }.allocate< MemoryPageType::PAGE_4K >( 1 ) ) - TASK_KERNEL_STACK_SIZE;
        main_pcb->kernel_stack_size = TASK_KERNEL_STACK_SIZE;
        // 设置内核栈
        auto kernel_stack = main_pcb->kernel_stack_base + main_pcb->kernel_stack_size;
        kernel_stack -= sizeof( PCB::ProcessContext );
        main_pcb->context.pcontext      = reinterpret_cast< PCB::ProcessContext      *>( kernel_stack );
        main_pcb->context.pcontext->rsp = CPU::get_rsp( );

        kernel_stack -= sizeof( PCB::ThreadContext );
        main_pcb->context.tcontext = reinterpret_cast< PCB::ThreadContext * >( kernel_stack );
        // 浮点栈放在PCB后面
        main_pcb->fpu_frame = reinterpret_cast< decltype( main_pcb->fpu_frame ) >( KHeapWalker { }.allocate( sizeof *main_pcb->fpu_frame ) );
        // 分配PID
        main_pcb->PID = pid_pool.allocate( );
        // 设置进程名
        std::strncpy( main_pcb->name, "SYSTEM", TASK_NAME_SIZE );
        // 暂时没啥用
        main_pcb->signal = 0;
        // 时间片越多优先级越高
        main_pcb->priority = Scheduler::Priority::IDLEPRIO;
        // 标注，例如进程还是线程，内核级别还是用户级别，FPU的情况等
        main_pcb->flags.fpu_enable = PCB::FpuEnable::ENABLE;
        main_pcb->flags.fpu_used   = PCB::FpuUsed::USED;
        main_pcb->flags.state      = PCB::State::RUNNING;
        main_pcb->flags.task_type  = PCB::Type::KERNEL_PROCESS;
#ifdef APIC
        // 当前cpu的id
        main_pcb->cpu_id = Interrupt::apic_id( );
#endif
        // 魔术字节
        main_pcb->stack_magic = PCB_STACK_MAGIC;

        SchedulerHelper { }.running_queue.append( main_pcb->general_task_node );
        main_pcb->general_task_node.container = main_pcb;
        main_pcb->jiffies                     = SchedulerHelper::rr_interval;
        main_pcb->PPID                        = 0;
        main_pcb->memory_manager.page_table   = reinterpret_cast< decltype( main_pcb->memory_manager.page_table )::page_table_entry   *>( CPU::get_page_table( ) );
        main_pcb->virtual_deadline            = SchedulerHelper::make_virtual_deadline( main_pcb->priority );
    }
    auto ProcessManager::get_running_task( VOID ) -> PCB * {
        // 得到正在运行的任务
        if ( ( main_pcb->context.pcontext->rsp & ~( TASK_KERNEL_STACK_SIZE - 1 ) ) == ( CPU::get_rsp( ) & ~( TASK_KERNEL_STACK_SIZE - 1 ) ) ) {
            return main_pcb;
        }
        auto pcb = reinterpret_cast< PCB * >( *reinterpret_cast< uint64_t * >( CPU::get_rsp( ) & ~( TASK_USER_STACK_SIZE - 1 ) ) );
        return pcb;
    }
}