#include <kernel/memory/heap/kheap/kheap_walker.hpp>
#include <kernel/memory/page/page_walker.hpp>
#include <kernel/task/general/pcb/pcb.hpp>
#include <kernel/task/task.hpp>
PUBLIC namespace QuantumNEC::Kernel {
    PCB::PCB( CONST char_t * _name_, uint64_t _priority_, Flags _flags_, VOID * _entry_, IN uint64_t _arg_ ) noexcept {
        // 内核栈处理
        this->kernel_stack_base = (uint64_t)PageWalker { }.allocate< MemoryPageType::PAGE_4K >( 1 );
        this->kernel_stack_size = TASK_KERNEL_STACK_SIZE;
        // 设置内核栈
        auto kernel_stack = this->kernel_stack_base + this->kernel_stack_size;
        kernel_stack -= sizeof( ProcessContext );
        this->context.pcontext = reinterpret_cast< ProcessContext * >( kernel_stack );

        // 设置用户栈
        this->user_stack_base = (uint64_t)PageWalker { }.allocate< MemoryPageType::PAGE_2M >( TASK_USER_STACK_SIZE / PageWalker::__page_size__< MemoryPageType::PAGE_2M > );     // 用户栈8M大小
        this->user_stack_size = TASK_USER_STACK_SIZE;

        if ( flags.task_type != Type::THREAD ) {
            // 不是线程，那就设置进程栈
            this->context.pcontext->make( _entry_, this->user_stack_base + this->user_stack_size - 1, _flags_.task_type );
        }     // 是线程，那就空着作为承载
        // 线程栈

        kernel_stack -= sizeof( ThreadContext );
        this->context.tcontext = reinterpret_cast< ThreadContext * >( kernel_stack );

        // if ( flags.task_type == Type::THREAD ) {
        //     // 是线程，就设置标准的线程栈
        //     this->context.tcontext->make( _entry_, _arg_ );
        // }
        // else {
        //     // 不是线程，就设置指向进程栈的线程栈
        //     // this->context.tcontext->make( (VOID *)Architecture::ArchitectureManager< TARGET_ARCH >::to_process, (uint64_t)this->context.pcontext );
        // }

        if ( flags.task_type == Type::USER_PROCESS ) {
            // 用户进程有自己的页表，所以复制内核页表
            this->memory_manager.page_table.copy( this->memory_manager.page_table );
            // 将这一段内存映射为用户页
            this->memory_manager.page_table.map( this->user_stack_base,
                                                 USER_STACK_VIRTUAL_ADDRESS_TOP - this->user_stack_size,
                                                 TASK_USER_STACK_SIZE / PageWalker::__page_size__< MemoryPageType::PAGE_2M >,
                                                 PAGE_PRESENT | PAGE_RW_W | PAGE_US_U,
                                                 MemoryPageType::PAGE_2M );
            // 用户栈栈底存PCB的地址
            *(uint64_t *)( USER_STACK_VIRTUAL_ADDRESS_TOP - this->user_stack_size ) = uint64_t( this );
            // 将RSP指向用户栈栈顶
            this->context.pcontext->rsp = USER_STACK_VIRTUAL_ADDRESS_TOP;
        }
        else {
            // 用户栈栈底存PCB的地址
            *(uint64_t *)physical_to_virtual( this->user_stack_base ) = uint64_t( this );
        }

        // 浮点栈放在PCB后面
        this->fpu_frame = reinterpret_cast< decltype( this->fpu_frame ) >( KHeapWalker { }.allocate( sizeof *this->fpu_frame ) );
        // 分配PID
        this->PID = pid_pool.allocate( );
        // 设置进程名
        std::strncpy( this->name, _name_, TASK_NAME_SIZE );
        // 暂时没啥用
        this->signal = 0;
        // 时间片越多优先级越高
        this->schedule.priority = _priority_;
        // 标注，例如进程还是线程，内核级别还是用户级别，FPU的情况等
        this->flags.fpu_enable          = _flags_.fpu_enable;
        this->flags.fpu_used            = _flags_.fpu_used;
        this->flags.state               = _flags_.state;
        this->flags.task_type           = _flags_.task_type;
        this->schedule.virtual_deadline = SchedulerHelper::make_virtual_deadline( this->schedule.priority );
#ifdef APIC
        // 当前cpu的id
        this->schedule.cpu_id = Interrupt::apic_id( );
#endif
        // 魔术字节
        this->stack_magic = PCB_STACK_MAGIC;
    }
    auto PCB::ProcessContext::make( IN VOID * _entry, IN uint64_t kernel_stack_top, IN Type type ) -> BOOL {
        this->rip = _entry;
        this->rsp = kernel_stack_top;
        if ( type == Type::KERNEL_PROCESS ) {
            this->cs          = SELECTOR_CODE64_KERNEL;
            this->ss          = SELECTOR_DATA64_KERNEL;
            this->regs.ds     = SELECTOR_DATA64_KERNEL;
            this->regs.es     = SELECTOR_DATA64_KERNEL;
            this->regs.gs     = SELECTOR_DATA64_KERNEL;
            this->regs.fs     = SELECTOR_DATA64_KERNEL;
            this->rflags.IOPL = 0;
            this->rflags.MBS  = 1;
            this->rflags.IF   = 1;
        }
        else {
            this->cs          = SELECTOR_CODE64_USER;
            this->ss          = SELECTOR_DATA64_USER;
            this->regs.ds     = SELECTOR_DATA64_USER;
            this->regs.es     = SELECTOR_DATA64_USER;
            this->regs.gs     = SELECTOR_DATA64_USER;
            this->regs.fs     = SELECTOR_DATA64_USER;
            this->rflags.IOPL = 3;
            this->rflags.MBS  = 1;
            this->rflags.IF   = 1;
        }
    }
    auto PCB::ProcessContext::operator=( CONST Interrupt::InterruptFrame &frame ) -> ProcessContext & {
        this->cs         = frame.cs;
        this->ss         = frame.ss;
        this->rip        = frame.rip;
        this->rsp        = frame.rsp;
        this->rflags     = frame.rflags;
        this->regs       = frame.regs;
        this->error_code = frame.error_code;
        this->old_rax    = frame.old_rax;
        this->vector     = frame.vector;
        this->handler    = frame.handler;
        return *this;
    }
    auto PCB::ThreadContext::make( VOID * entry, IN uint64_t _arg ) -> BOOL {
    }
}
