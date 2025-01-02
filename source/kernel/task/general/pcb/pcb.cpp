#include <kernel/memory/heap/kheap/kheap_walker.hpp>
#include <kernel/memory/memory.hpp>
#include <kernel/memory/page/page_walker.hpp>
#include <kernel/task/general/pcb/pcb.hpp>
#include <kernel/task/task.hpp>
namespace QuantumNEC::Kernel {
PCB::PCB( const char_t *_name_, uint64_t _priority_, Flags _flags_, void *_entry_, IN uint64_t _arg_ ) noexcept {
    // 内核栈处理
    this->kernel_stack_base = (uint64_t)PageWalker { }.allocate< MemoryPageType::PAGE_4K >( 1 );
    this->kernel_stack_size = TASK_KERNEL_STACK_SIZE;
    // 设置内核栈
    auto kernel_stack = this->kernel_stack_base + this->kernel_stack_size;
    kernel_stack -= sizeof( ProcessContext );
    this->context.pcontext = reinterpret_cast< ProcessContext * >( physical_to_virtual( kernel_stack ) );
    // 设置用户栈
    this->user_stack_base = (uint64_t)PageWalker { }.allocate< MemoryPageType::PAGE_2M >( TASK_USER_STACK_SIZE / PageWalker::__page_size__< MemoryPageType::PAGE_2M > );     // 用户栈8M大小
    this->user_stack_size = TASK_USER_STACK_SIZE;

    if ( _flags_.task_type != Flags::Type::THREAD ) {
        // 不是线程，那就设置进程栈
        this->context.pcontext->make( _entry_, (uint64_t)physical_to_virtual( this->user_stack_base ) + this->user_stack_size - 1, _flags_.task_type );

    }     // 是线程，那就空着作为承载
    // 线程栈

    kernel_stack -= sizeof( ThreadContext );
    this->context.tcontext = reinterpret_cast< ThreadContext * >( physical_to_virtual( kernel_stack ) );

    if ( flags.task_type == Flags::Type::THREAD ) {
        // 是线程，就设置标准的线程栈
        this->context.tcontext->make( _entry_, _arg_ );
    }
    else {
        // 不是线程，就设SchedulerHelper::task_queue[ pcb->schedule.priority ].append( pcb->schedule.general_task_node );置指向进程栈的线程栈
        // this->context.tcontext->make( (VOID *)Architecture::ArchitectureManager< TARGET_ARCH >::to_process, (uint64_t)this->context.pcontext );
    }

    this->memory_manager.install( );
    if ( _flags_.task_type == Flags::Type::USER_PROCESS ) {
        // 用户进程有自己的页表，所以复制内核页表
        this->memory_manager.page_table->copy( *x86_64::Paging::kernel_page_table );
        // map user stack and set r/w, u/s, p
        this->memory_manager.page_table->map( this->user_stack_base,
                                              this->memory_manager.page_table->USER_STACK_VIRTUAL_ADDRESS_TOP - this->user_stack_size + 1,
                                              TASK_USER_STACK_SIZE / PageWalker::__page_size__< MemoryPageType::PAGE_2M >,
                                              this->memory_manager.page_table->PAGE_PRESENT | this->memory_manager.page_table->PAGE_RW_W | this->memory_manager.page_table->PAGE_US_U,
                                              MemoryPageType::PAGE_2M );

        // set rsp to point user stack top
        this->context.pcontext->rsp = this->memory_manager.page_table->USER_STACK_VIRTUAL_ADDRESS_TOP;
    }

    // 栈栈底存PCB的地址
    *(uint64_t *)physical_to_virtual( this->kernel_stack_base ) = uint64_t( this );

    // 浮点栈放在PCB后面
    this->fpu_frame = reinterpret_cast< decltype( this->fpu_frame ) >( KHeapWalker { }.allocate( sizeof *this->fpu_frame ) );
    // 分配PID
    this->PID = pid_pool.allocate( );
    // 设置进程名
    std::strncpy( this->name, _name_, TASK_NAME_SIZE );

    // 时间片越多优先级越高
    this->schedule.priority                    = _priority_;
    this->schedule.general_task_node.container = this;
    // 标注，例如进程还是线程，内核级别还是用户级别，FPU的情况等
    this->flags                     = _flags_;
    this->schedule.virtual_deadline = SchedulerHelper::make_virtual_deadline( this->schedule.priority );
    this->schedule.jiffies          = SchedulerHelper::rr_interval;
    // 当前cpu的id
    this->schedule.cpu_id = Interrupt::cpu_id( );
    // 魔术字节
    this->stack_magic = PCB_STACK_MAGIC;
}
auto PCB::ProcessContext::make( IN void *_entry, IN uint64_t stack_top, IN Flags::Type type ) -> bool {
    this->rip = _entry;
    this->rsp = stack_top;
    if ( type == Flags::Type::KERNEL_PROCESS ) {
#if defined( __x86_64__ )
        this->cs          = x86_64::SELECTOR_CODE64_KERNEL;
        this->ss          = x86_64::SELECTOR_DATA64_KERNEL;
        this->regs.ds     = x86_64::SELECTOR_DATA64_KERNEL;
        this->regs.es     = x86_64::SELECTOR_DATA64_KERNEL;
        this->regs.gs     = x86_64::SELECTOR_DATA64_KERNEL;
        this->regs.fs     = x86_64::SELECTOR_DATA64_KERNEL;
        this->rflags.IOPL = 3;
        this->rflags.MBS  = 1;
        this->rflags.IF   = 1;
#elif defined( __aarch64__ )
#endif
        return true;
    }
    else {
#if defined( __x86_64__ )
        this->cs          = x86_64::SELECTOR_CODE64_USER;
        this->ss          = x86_64::SELECTOR_DATA64_USER;
        this->regs.ds     = x86_64::SELECTOR_DATA64_USER;
        this->regs.es     = x86_64::SELECTOR_DATA64_USER;
        this->regs.gs     = x86_64::SELECTOR_DATA64_USER;
        this->regs.fs     = x86_64::SELECTOR_DATA64_USER;
        this->rflags.IOPL = 0;
        this->rflags.MBS  = 1;
        this->rflags.IF   = 1;
#elif defined( __aarch64__ )
#endif
        return true;
    }

    return false;
}

auto PCB::ThreadContext::make( void *entry, IN uint64_t _arg ) -> bool {
    return true;
}
}     // namespace QuantumNEC::Kernel
