#include <kernel/task/manager/pcb.hpp>
PUBLIC namespace QuantumNEC::Kernel {
    PCB::PCB( CONST char_t * _name_, uint64_t _priority_, Flags _flags_, VOID * _entry_, IN uint64_t _arg_ ) noexcept {
        // 内核栈处理
        this->kernel_stack_base = (uint64_t)PageAllocater { }.allocate< MemoryPageType::PAGE_4K >( 1 );
        this->kernel_stack_size = PageAllocater::__page_size__< MemoryPageType::PAGE_4K >;
        // 设置内核栈
        auto kernel_stack = this->kernel_stack_base + this->kernel_stack_size;
        kernel_stack -= sizeof( ProcessContext );
        this->context.pcontext = reinterpret_cast< ProcessContext * >( kernel_stack );
        if ( flags.task_type != Type::THREAD ) {
            // 不是线程，那就设置进程栈
            this->context.pcontext->make( _entry_, this->kernel_stack_base + this->kernel_stack_size );
        }     // 是线程，那就空着作为承载

        kernel_stack -= sizeof( ThreadContext );
        this->context.tcontext = reinterpret_cast< ThreadContext * >( kernel_stack );
        if ( flags.task_type == Type::THREAD ) {
            // 是线程，就设置标准的线程栈
            this->context.tcontext->make( _entry_, _arg_ );
        }
        else {
            // 不是线程，就设置指向进程栈的线程栈
            // this->context.tcontext->make( (VOID *)Architecture::ArchitectureManager< TARGET_ARCH >::to_process, (uint64_t)this->context.pcontext );
        }

        // 用户栈
        if ( flags.task_type == Type::USER_PROCESS ) {
            // 用户进程有自己的页表，所以复制内核页表
            PageTableWalker { }.copy( this->memory_manager.page_table );
            // 用户栈8M大小
            this->user_stack_base = (uint64_t)PageAllocater { }.allocate< MemoryPageType::PAGE_2M >( 4 );
            this->user_stack_size = PageAllocater::__page_size__< MemoryPageType::PAGE_2M > * 4;
            // 用户栈栈底存PCB的地址
            *(uint64_t *)physical_to_virtual( this->user_stack_base ) = uint64_t( this );
            // 将这一段内存映射为用户页
            PageTableWalker { }.map( this->user_stack_base,
                                     USER_STACK_VIRTUAL_ADDRESS_TOP - this->user_stack_size,
                                     4,
                                     PAGE_PRESENT | PAGE_RW_W | PAGE_US_U,
                                     MemoryPageType::PAGE_2M,
                                     this->memory_manager.page_table );
            // 将RSP指向用户栈栈顶
            this->context.pcontext->rsp = USER_STACK_VIRTUAL_ADDRESS_TOP;
        }
        // 浮点栈放在PCB后面
        this->fpu_frame = reinterpret_cast< FloatPointUnit::FpuFrame * >( this + 1 );
        // 分配PID
        this->PID = pid_pool.allocate( );
        // 设置进程名
        std::strncpy( this->name, _name_, TASK_NAME_SIZE );
        // 暂时没啥用
        this->signal = 0;
        // 时间片越多优先级越高
        this->priority = _priority_;
        // 标注，例如进程还是线程，内核级别还是用户级别，FPU的情况等
        this->flags.fpu_enable = _flags_.fpu_enable;
        this->flags.fpu_used   = _flags_.fpu_used;
        this->flags.state      = _flags_.state;
        this->flags.task_type  = _flags_.task_type;
#ifdef APIC
        // 当前cpu的id
        this->cpu_id = Interrupt::apic_id( );
#endif
        // 魔术字节
        this->stack_magic = TASK_STACK_MAGIC;
    }
}