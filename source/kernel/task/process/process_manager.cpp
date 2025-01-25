#include <kernel/cpu/cpu.hpp>
#include <kernel/memory/heap/kheap/kheap_allocater.hpp>
#include <kernel/memory/memory.hpp>
#include <kernel/task/process/process_manager.hpp>
#include <kernel/task/task.hpp>
namespace QuantumNEC::Kernel {
auto ProcessManager::main_process_install( uint64_t core ) -> PCB * {
    KHeapAllocator< PCB > pcb_allocator;

    auto pcb = std::allocator_traits< decltype( pcb_allocator ) >::allocate( pcb_allocator, 1 );

    // 当前cpu的id
    pcb->schedule.cpu_id = Interrupt::cpu_id( );
    // 内核栈处理
    pcb->kernel_stack_base = (uint64_t)virtual_to_physical( CPU::get_rsp( ) & ~( TASK_KERNEL_STACK_SIZE - 1 ) );
    pcb->kernel_stack_size = TASK_KERNEL_STACK_SIZE;

#if defined( __x86_64__ )
    x86_64::GlobalSegmentDescriptorTable::gdt->tss[ pcb->schedule.cpu_id ].set_rsp0( (uint64_t)virtual_to_physical( pcb->kernel_stack_base + pcb->kernel_stack_size ) );
#endif

    *( (uint64_t *)physical_to_virtual( pcb->kernel_stack_base ) ) = (uint64_t)pcb;

    // 分配PID
    pcb->PID = pid_pool.allocate( );
    // 设置进程名
    std::strncpy( pcb->name, "SYSTEM", TASK_NAME_SIZE );
    // 时间片越多优先级越高
    pcb->schedule.priority = Scheduler::Priority::IDLEPRIO;
    // 标注，例如进程还是线程，内核级别还是用户级别，FPU的情况等
    pcb->flags.fpu_enable = PCB::__flags__::__fpu_state__::ENABLE;
    pcb->flags.fpu_used   = PCB::__flags__::__fpu_state__::USED;
    pcb->schedule.state   = Scheduler::Schedule::State::RUNNING;
    pcb->flags.task_type  = PCB::__flags__::__type__::KERNEL_PROCESS;

    // 魔术字节
    pcb->stack_magic                = PCB_STACK_MAGIC;
    pcb->schedule.general_task_node = *pcb;
    pcb->schedule.jiffies           = SchedulerHelper::make_jiffies( pcb->schedule.priority );
    pcb->schedule.virtual_deadline  = SchedulerHelper::make_virtual_deadline( pcb->schedule.priority, pcb->schedule.jiffies );
    pcb->schedule.cpu_id            = core;
    // 暂时没啥用
    pcb->schedule.signal = 0;
    pcb->PPID            = 0;

    pcb->memory_manager.page_table = (uint64_t)x86_64::Paging::kernel_page_table->get_table( );
    KHeapAllocator< FloatPointUnit::FpuFrame > fpu_frame_allocater;

    pcb->fpu_frame = std::allocator_traits< decltype( fpu_frame_allocater ) >::allocate( fpu_frame_allocater, 1 );
    SchedulerHelper::running_queue.append( pcb->schedule.general_task_node );

    return pcb;
}
ProcessManager::ProcessManager( void ) noexcept {
    auto pcb       = this->main_process_install( 0 );
    this->main_pcb = pcb;
}

_C_LINK auto get_current_kernel_stack_top_in_user_mode( void ) -> uint64_t {
    return x86_64::GlobalSegmentDescriptorTable::gdt->tss[ Interrupt::cpu_id( ) ].get_rsp0( );
}

}     // namespace QuantumNEC::Kernel