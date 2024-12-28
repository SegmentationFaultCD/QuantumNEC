#include <kernel/cpu/cpu.hpp>
#include <kernel/memory/heap/kheap/kheap_walker.hpp>
#include <kernel/memory/page/page_walker.hpp>
#include <kernel/task/process/process_manager.hpp>
#include <kernel/task/task.hpp>
namespace QuantumNEC::Kernel {
auto ProcessManager::main_process_install( uint64_t ) -> PCB * {
    auto pcb = reinterpret_cast< PCB * >( KHeapWalker { }.allocate( sizeof( PCB ) ) );
    // 内核栈处理
    pcb->kernel_stack_base = CPU::get_rsp( ) & ~( TASK_KERNEL_STACK_SIZE - 1 );
    pcb->kernel_stack_size = TASK_KERNEL_STACK_SIZE;

    pcb->context.pcontext                   = (PCB::ProcessContext *)( pcb->kernel_stack_base + pcb->kernel_stack_size - sizeof( PCB::ProcessContext ) );
    pcb->context.tcontext                   = reinterpret_cast< PCB::ThreadContext                   *>( pcb->context.pcontext ) - 1;
    *( (uint64_t *)pcb->kernel_stack_base ) = (uint64_t)pcb;
    // 分配PID
    pcb->PID = pid_pool.allocate( );
    // 设置进程名
    std::strncpy( pcb->name, "SYSTEM", TASK_NAME_SIZE );
    // 时间片越多优先级越高
    pcb->schedule.priority = Scheduler::Priority::IDLEPRIO;
    // 标注，例如进程还是线程，内核级别还是用户级别，FPU的情况等
    pcb->flags.fpu_enable = PCB::Flags::Fpu::ENABLE;
    pcb->flags.fpu_used   = PCB::Flags::Fpu::USED;
    pcb->schedule.state   = Scheduler::Schedule::State::RUNNING;
    pcb->flags.task_type  = PCB::Flags::Type::KERNEL_PROCESS;

    // 当前cpu的id
    pcb->schedule.cpu_id = Interrupt::cpu_id( );
    // 魔术字节
    pcb->stack_magic                          = PCB_STACK_MAGIC;
    pcb->schedule.general_task_node.container = pcb;
    pcb->schedule.jiffies                     = SchedulerHelper::rr_interval;
    pcb->schedule.cpu_id                      = Interrupt::cpu_id( );
    // 暂时没啥用
    pcb->schedule.signal = 0;
    pcb->PPID            = 0;

    pcb->memory_manager.install( );
    *pcb->memory_manager.page_table = (uint64_t)x86_64::Paging::kernel_page_table->get_table( );
    pcb->schedule.virtual_deadline  = SchedulerHelper::make_virtual_deadline( pcb->schedule.priority );

    SchedulerHelper::running_queue.append( pcb->schedule.general_task_node );
    return pcb;
}
ProcessManager::ProcessManager( void ) noexcept {
    auto pcb       = this->main_process_install( 0 );
    pcb->fpu_frame = reinterpret_cast< decltype( pcb->fpu_frame ) >( KHeapWalker { }.allocate( sizeof *main_pcb->fpu_frame ) );
    this->main_pcb = pcb;
}
auto ProcessManager::get_running_task( void ) -> PCB * {
    // 得到正在运行的任务
    auto kstack = CPU::get_rsp( ) & ~( TASK_KERNEL_STACK_SIZE - 1 );
    return (PCB *)( *( (uint64_t *)kstack ) );
}
_C_LINK auto get_current_kernel_stack_top_in_user_mode( void ) -> uint64_t {
    return x86_64::GlobalSegmentDescriptorTable::gdt->tss[ Interrupt::cpu_id( ) ].get_rsp0( );
}
}     // namespace QuantumNEC::Kernel