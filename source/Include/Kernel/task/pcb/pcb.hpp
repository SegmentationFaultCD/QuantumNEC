#pragma once
#include "Kernel/memory/paging_map/pmlxt.hpp"
#include <Kernel/print.hpp>
#include <Lib/Uefi.hpp>
#include <Lib/list.hpp>
#include <Lib/spin_lock.hpp>

#include <Kernel/task/pcb/pid.hpp>
#include <Kernel/memory/memory.hpp>
#include <Arch/Arch.hpp>
#include <Libcxx/cstring.hpp>
#include <concepts>
#include <cstdint>

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

    PUBLIC struct PCB
    {
    public:
        struct MM
        {
            pml4t page_table;     // 任务所持有的页表地址
            explicit MM( VOID ) = default;
            auto operator=( MM &mm ) -> MM & {
                this->page_table = mm.page_table;
                return *this;
            }
        };
        struct ThreadContext     // 栈底
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
            auto make( IN VOID *_entry, IN uint64_t _arg ) -> BOOL;
            // 栈顶
        } _packed;
        struct ProcessContext :
            Architecture::ArchitectureManager< TARGET_ARCH >::InterruptFrame
        {
            auto make( IN VOID *_entry, IN uint64_t kernel_stack_top ) -> BOOL;
        };
        struct Context
        {
            ProcessContext *pcontext;
            ThreadContext *tcontext;
            explicit Context( VOID ) = default;
        };

        PUBLIC enum TaskStatus : uint64_t {
            RUNNING = ( 1 << 0 ),
            READY = ( 1 << 1 ),
            BLOCKED = ( 1 << 2 ),
            SENDING = ( 1 << 3 ),
            RECEIVING = ( 1 << 4 ),
            WAITING = ( 1 << 5 ),
            HANGING = ( 1 << 6 ),
            DIED = ( 1 << 7 ),
        };

    public:
        Lib::ListNode general_task_node;     // 通用任务队列 连接每个任务

        MM memory_manager;     // 记录内存分布

        uint64_t kernel_stack_base;     // 内核栈栈底
        uint64_t kernel_stack_size;     // 内核栈大小
        uint64_t user_stack_base;       // 用户栈栈底
        uint64_t user_stack_size;       // 用户栈大小
        Context context;                // 上下文 记录寄存器状态

        Architecture::Message message;     // 进程消息体

        uint64_t PID;                      // 任务ID
        uint64_t PPID;                     // 父进程ID
        char_t name[ TASK_NAME_SIZE ];     // 任务名
        uint64_t status;                   // 任务状态
        uint64_t jiffies;                  // 可运行的时间片
        uint64_t signal;                   // 任务持有的信号
        uint64_t priority;                 // 任务优先级
        uint64_t flags;                    // 任务特殊标志
        uint64_t virtual_deadline;         // 虚拟截止时间
        uint64_t cpu_id;                   // CPU ID

        Lib::ListTable thread_queue;     // 进程持有的线程队列
        uint64_t number_of_threads;      // 线程数量，最少为1（也就是进程本身）

        Architecture::ArchitectureManager< TARGET_ARCH >::SSE *fxsave_region;
        uint64_t stack_magic;     // 用于检测栈的溢出
        explicit PCB( IN CONST char_t *_name, IN int64_t _priority, IN int64_t _flags, IN VOID *_entry, IN uint64_t _arg ) noexcept {
            Lib::SpinLock lock { };
            lock.acquire( );

            // 内核栈处理
            this->kernel_stack_base = reinterpret_cast< uint64_t >( this ) + TASK_STACK_SIZE - PAGE_4K_SIZE;
            this->kernel_stack_size = PAGE_4K_SIZE;
            auto kernel_stack = this->kernel_stack_base + this->kernel_stack_size;
            kernel_stack -= sizeof( ProcessContext );
            this->context.pcontext = reinterpret_cast< ProcessContext * >( kernel_stack );
            if ( !( _flags & TASK_FLAG_THREAD ) ) {
                this->context.pcontext->make( _entry, this->kernel_stack_base + this->kernel_stack_size );
            }
            kernel_stack -= sizeof( ThreadContext );
            this->context.tcontext = reinterpret_cast< ThreadContext * >( kernel_stack );
            if ( _flags & TASK_FLAG_THREAD ) {
                this->context.tcontext->make( _entry, _arg );
            }
            else {
                this->context.tcontext->make( (VOID *)Architecture::ArchitectureManager< TARGET_ARCH >::to_process, (uint64_t)this->context.pcontext );
            }
            // 用户栈
            if ( _flags & TASK_FLAG_USER_PROCESS ) {
                Memory::memory_paging_map->copy( this->memory_manager.page_table );     // 制作页表
                this->user_stack_base = (uint64_t)Memory::page->allocate( 4, MemoryPageType::PAGE_2M );
                this->user_stack_size = PAGE_2M_SIZE * 4;
                *(uint64_t *)physical_to_virtual( this->user_stack_base ) = uint64_t( this );
                Memory::memory_paging_map->map( this->user_stack_base,
                                                USER_STACK_VIRTUAL_ADDRESS_TOP - this->user_stack_size,
                                                4,
                                                PAGE_PRESENT | PAGE_RW_W | PAGE_US_U,
                                                MemoryPageType::PAGE_2M,
                                                this->memory_manager.page_table );
                this->context.pcontext->rsp = USER_STACK_VIRTUAL_ADDRESS_TOP;
            }
            // SSE
            this->fxsave_region = reinterpret_cast< Architecture::ArchitectureManager< TARGET_ARCH >::SSE * >( this + 1 );
            // 分配PID
            this->PID = pid_pool.allocate( );
            // 设置进程名
            std::strncpy( this->name, _name, TASK_NAME_SIZE );
            // 设置进程初始状态
            this->status |= static_cast< uint64_t >( TaskStatus::READY );
            // 暂时没啥用
            this->signal = 0;
            // 时间片越多优先级越高
            this->priority = _priority;
            // 标注，例如进程还是线程，内核级别还是用户级别，FPU的情况等
            this->flags = _flags;

#ifdef APIC
            // 当前cpu的id
            this->cpu_id = Architecture::ArchitectureManager< TARGET_ARCH >::apic_id( );
#endif
            // 魔术字节
            this->stack_magic = TASK_STACK_MAGIC;

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
            Architecture::ArchitectureManager< TARGET_ARCH >::gdt->tss[ 0 ].set_rsp0( this->kernel_stack_base + this->kernel_stack_size );

#elif defined( __aarch64__ )
#else
#error Not any registers
#endif
            Memory::memory_paging_map->activate( this->memory_manager.page_table );

            lock.release( );
        }
        auto operator=( PCB &pcb ) -> PCB & {
            this->general_task_node = pcb.general_task_node;
            this->memory_manager = pcb.memory_manager;
            this->kernel_stack_base = pcb.kernel_stack_base;
            this->kernel_stack_size = pcb.kernel_stack_size;
            this->user_stack_base = pcb.user_stack_base;
            this->user_stack_size = pcb.user_stack_size;
            this->context = pcb.context;
            this->message = pcb.message;
            this->PID = pcb.PID;
            this->PID = pcb.PPID;
            std::strcpy( this->name, pcb.name );
            this->status = pcb.status;
            this->jiffies = pcb.jiffies;
            this->signal = pcb.signal;
            this->priority = pcb.priority;
            this->flags = pcb.flags;
            this->virtual_deadline = pcb.virtual_deadline;
            this->cpu_id = pcb.cpu_id;
            this->thread_queue = pcb.thread_queue;
            this->number_of_threads = pcb.number_of_threads;
            this->fxsave_region = pcb.fxsave_region;
            this->stack_magic = pcb.stack_magic;
            return *this;
        }
    };

    inline auto get_current( VOID ) {
        pml4t page_table { (pml4t_entry *)Architecture::ArchitectureManager< TARGET_ARCH >::get_page_table( ) };
        auto rsp = Architecture::ArchitectureManager< TARGET_ARCH >::get_rsp( );
        rsp = (uint64_t)Memory::memory_paging_map->VTP_address_from( (VOID *)rsp, MemoryPageType::PAGE_2M, page_table );
        return (PCB *)( *(uint64_t *)rsp );
    }
}