#pragma once
#include <Lib/Uefi.hpp>
#include <Lib/list.hpp>
#include <Lib/spin_lock.hpp>
#include <concepts>
#include <Kernel/task/pid.hpp>
#include <Kernel/memory/memory.hpp>
#include <Arch/Arch.hpp>
#include <Lib/allocate.hpp>
#include <Libcxx/cstring.hpp>
PUBLIC namespace QuantumNEC::Kernel {
    PUBLIC constexpr CONST auto TASK_STACK_SIZE { 65536ull };     // 64KB
    PUBLIC constexpr CONST auto TASK_STACK_MAGIC { 0x1145141919810ULL };
    PUBLIC constexpr CONST auto TASK_NAME_SIZE { 32ull };
    PUBLIC constexpr CONST auto TASK_FLAG_THREAD { 1ull << 0 };
    PUBLIC constexpr CONST auto TASK_FLAG_KERNEL_PROCESS { 1ull << 1 };
    PUBLIC constexpr CONST auto TASK_FLAG_USER_PROCESS { 0ull << 1 };
    PUBLIC constexpr CONST auto TASK_FLAG_FPU_USED { 1ull << 2 };
    PUBLIC constexpr CONST auto TASK_FLAG_FPU_UNUSED { 1ull << 3 };
    PUBLIC constexpr CONST auto TASK_FLAG_FPU_ENABLE { 1ull << 4 };

    PUBLIC using TaskArg = uint64_t;
    PUBLIC using TaskReturnValue = int64_t;
    PUBLIC using TaskFunction = FuncPtr< TaskReturnValue, TaskArg >;

    PUBLIC struct ThreadContext     // 栈底
    {
#if defined( __x86_64__ )
        // ABI要求保护的寄存器
        uint64_t r15;
        uint64_t r14;
        uint64_t r13;
        uint64_t r12;
        uint64_t rbp;
        uint64_t rbx;
        uint64_t rdi;
        uint64_t rsi;
        Architecture::RFlags rflags;
        VOID *rip;
        // 栈顶
#elif defined( __aarch64__ )
#else
#error Not any registers
#endif
        auto make( IN TaskFunction _entry, IN TaskArg _arg ) -> BOOL;
        // 栈顶
    } _packed;
    PUBLIC struct ProcessContext : Architecture::InterruptDescriptorTable::InterruptFrame
    {
        auto make( IN TaskFunction _entry ) -> BOOL;
    };
    PUBLIC struct TaskContext
    {
        ProcessContext *pcontext;
        ThreadContext *tcontext;
    };

    PUBLIC enum class TaskStatus : uint64_t {
        RUNNING = ( 1 << 0 ),
        READY = ( 1 << 1 ),
        BLOCKED = ( 1 << 2 ),
        SENDING = ( 1 << 3 ),
        RECEIVING = ( 1 << 4 ),
        WAITING = ( 1 << 5 ),
        HANGING = ( 1 << 6 ),
        DIED = ( 1 << 7 ),
    };

    PUBLIC constexpr CONST auto PRIORITY_QUEUE_COUNT { 103 };
    PUBLIC constexpr CONST auto DEADLINE { 31 };
    PUBLIC struct TaskPCB
    {
        // 连接每个任务
        Lib::ListNode general_task_node;     // 通用任务队列
        // 记录内存分布
        pml4t page_table;                 // 任务所持有的页表地址
        Lib::Allocate allocate_table;     // 虚拟地址表，在page_directory不为空时有效
        uint64_t kernel_stack_base;
        uint64_t kernel_stack_size;     //
        uint64_t user_stack_base;
        uint64_t user_stack_size;     //
        // 记录寄存器状态
        TaskContext context;     // 上下文
        // 进程间通信需要的
        Architecture::Message message;     // 进程消息体
        // 任务信息
        uint64_t PID;                      // 任务ID
        uint64_t PPID;                     // 父进程ID
        char_t name[ TASK_NAME_SIZE ];     // 任务名
        uint64_t status;                   // 任务状态
        uint64_t jiffies;                  // 每次在处理器上执行的时间嘀嗒数
        uint64_t signal;                   // 任务持有的信号
        uint64_t priority;                 // 任务优先级
        uint64_t flags;                    // 任务特殊标志
        uint64_t vd;                       // 虚拟截止时间
        uint64_t cpu_id;                   // CPU ID
        Architecture::ArchitectureManager< TARGET_ARCH >::SSE *fxsave_region;
        uint64_t stack_magic;     // 用于检测栈的溢出
        explicit TaskPCB( IN CONST char_t *_name, IN int64_t _priority, IN int64_t _flags, IN TaskFunction _entry, IN TaskArg _arg ) noexcept {
            Lib::SpinLock lock { };
            lock.acquire( );
            this->kernel_stack_base = reinterpret_cast< uint64_t >( this ) + TASK_STACK_SIZE - PAGE_4K_SIZE;
            this->kernel_stack_size = PAGE_4K_SIZE;
            auto kernel_stack = this->kernel_stack_base + this->kernel_stack_size;
            kernel_stack -= sizeof( ProcessContext );
            this->context.pcontext = reinterpret_cast< decltype( this->context.pcontext ) >( kernel_stack );
            if ( !( _flags & TASK_FLAG_THREAD ) ) {
                this->context.pcontext->make( _entry );
            }
            kernel_stack -= sizeof( ThreadContext );
            this->context.tcontext = reinterpret_cast< decltype( this->context.tcontext ) >( kernel_stack );
            if ( _flags & TASK_FLAG_THREAD ) {
                this->context.tcontext->make( _entry, _arg );
            }
            else {
                this->context.tcontext->make( (TaskFunction)Architecture::ArchitectureManager< TARGET_ARCH >::to_process, (TaskArg)this->context.pcontext );
            }

            this->fxsave_region = reinterpret_cast< Architecture::ArchitectureManager< TARGET_ARCH >::SSE * >( this + 1 );
            // 设置消息
            this->message.send_to = Architecture::NO_TASK;
            this->message.receive_from = Architecture::NO_TASK;
            // 分配PID
            this->PID = pid_pool.allocate( );
            // 设置进程名
            Lib::strncpy( this->name, _name, TASK_NAME_SIZE );
            // 设置进程初始状态
            this->status |= static_cast< uint64_t >( TaskStatus::READY );
            // 这个进程的使用时间
            this->jiffies = DEADLINE;
            // 暂时没啥用
            this->signal = 0;
            // 时间片越多优先级越高
            this->priority = _priority;
            // 标注，例如进程还是线程，内核级别还是用户级别，FPU的情况等
            this->flags = _flags;
            // 计算公式
            this->vd = Architecture::ArchitectureManager< TARGET_ARCH >::global_jiffies + ( _priority * DEADLINE );
#ifdef APIC
            // 当前cpu的id
            this->cpu_id = Architecture::ArchitectureManager< TARGET_ARCH >::apic_id( );
#endif
            // 魔术字节
            this->stack_magic = TASK_STACK_MAGIC;

            // 制作页表
            if ( _flags & TASK_FLAG_USER_PROCESS ) {
                Memory::memory_paging_map->copy( this->page_table );
                Memory::memory_paging_map->map( (uint64_t)virtual_to_physical( this ), (uint64_t)this, 1, PAGE_PRESENT | PAGE_RW_W | PAGE_US_U, MemoryPageType::PAGE_2M, this->page_table );

                this->user_stack_base = (uint64_t)Memory::page->malloc( 1, MemoryPageType::PAGE_2M );
                this->user_stack_size = PAGE_2M_SIZE;
                *(uint64_t *)physical_to_virtual( this->user_stack_base ) = uint64_t( this );
                Memory::memory_paging_map->map( this->user_stack_base, USER_STACK_VIRTUAL_ADDRESS_TOP - this->user_stack_size, 1, PAGE_PRESENT | PAGE_RW_W | PAGE_US_U, MemoryPageType::PAGE_2M, this->page_table );
                this->context.pcontext->rsp = USER_STACK_VIRTUAL_ADDRESS_TOP;
            }
            // this->allocate_table = Lib::Base::Allocate { sizeof( Lib::Base::Allocate ::AllocateTableEntry ) * 1024 };
            // this->allocate_table.free( USER_STACK_VIRTUAL_START_ADDRESS / PAGE_SIZE, ( USER_STACK_VIRTUAL_END_ADDRESS - USER_STACK_VIRTUAL_START_ADDRESS ) / PAGE_SIZE );

            lock.release( );
        }

        /**
         * @brief 激活任务
         * @param pcb 要激活的任务PCB
         */

        auto activate( VOID ) {
            Lib::SpinLock lock { };

            lock.acquire( );
#if defined( __x86_64__ )
            Architecture::ArchitectureManager< TARGET_ARCH >::gdt->tss[ 0 ].set_rsp0< TASK_STACK_SIZE >( this );

#elif defined( __aarch64__ )
#else
#error Not any registers
#endif
            Memory::memory_paging_map->activate( this->page_table );
            lock.release( );
        }
    };

    inline auto get_current( VOID ) {
        pml4t page_table { (pml4t_entry *)Architecture::ArchitectureManager< TARGET_ARCH >::get_page_table( ) };
        auto rsp = Architecture::ArchitectureManager< TARGET_ARCH >::get_rsp( );
        rsp = (uint64_t)Memory::memory_paging_map->VTP_address_from( (VOID *)rsp, MemoryPageType::PAGE_2M, page_table );
        return (TaskPCB *)( *(uint64_t *)rsp );
        // return reinterpret_cast< PCB * >( Architecture::ArchitectureManager< TARGET_ARCH >::get_rsp( ) & ~( TASK_STACK_SIZE - 1 ) );
    }
}