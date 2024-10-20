#pragma once
#include <kernel/print.hpp>
#include <lib/Uefi.hpp>
#include <lib/list.hpp>
#include <lib/spin_lock.hpp>
#include <kernel/syscall/message.hpp>
#include <kernel/task/manager/pid.hpp>
#include <kernel/memory/memory.hpp>
#include <libcxx/cstring.hpp>
#include <concepts>
#include <cstdint>
#include <tuple>
#include <kernel/driver/driver.hpp>
#include <kernel/cpu/cpu.hpp>
PUBLIC namespace QuantumNEC::Kernel {
    PUBLIC constexpr CONST auto TASK_STACK_SIZE { 65536ull };     // 64KB
    PUBLIC constexpr CONST auto TASK_STACK_MAGIC { 0x1145141919810ULL };
    PUBLIC constexpr CONST auto TASK_NAME_SIZE { 32ull };

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
            RFlags rflags;
            VOID *rip;
            // 栈顶
#elif defined( __aarch64__ )
#else
#error Not any registers
#endif
            auto make( IN VOID *_entry, IN uint64_t _arg ) -> BOOL;
            // 栈顶
        } _packed;
        struct ProcessContext : Interrupt::InterruptFrame
        {
            auto make( IN VOID *_entry, IN uint64_t kernel_stack_top ) -> BOOL;
        };
        struct Context
        {
            ProcessContext *pcontext;
            ThreadContext *tcontext;
            explicit Context( VOID ) = default;
        };

        enum class State : uint64_t {
            RUNNING = 0,
            READY = 1,
            BLOCKED = 2,
            SENDING = 3,
            RECEIVING = 4,
            WAITING = 5,
            HANGING = 6,
            DIED = 7,
        };
        enum class FpuEnable : uint64_t {
            ENABLE = 1,
            DISABLE = 0
        };
        enum class FpuUsed : uint64_t {
            USED = 1,
            UNUSED = 0,
        };
        enum class Type : uint64_t {
            THREAD = 0,
            KERNEL_PROCESS = 1,
            USER_PROCESS = 2
        };

    public:
        Lib::ListNode general_task_node;     // 通用任务队列 连接除running状态的每个任务

        MM memory_manager;     // 记录内存分布

        uint64_t kernel_stack_base;     // 内核栈栈底
        uint64_t kernel_stack_size;     // 内核栈大小
        uint64_t user_stack_base;       // 用户栈栈底
        uint64_t user_stack_size;       // 用户栈大小
        Context context;                // 上下文 记录寄存器状态

        Message message;     // 进程消息体

        uint64_t PID;                      // 任务ID
        uint64_t PPID;                     // 父进程ID
        char_t name[ TASK_NAME_SIZE ];     // 任务名

        uint64_t jiffies;      // 可运行的时间片
        uint64_t signal;       // 任务持有的信号
        uint64_t priority;     // 任务优先级

        struct
        {
            State state : 7;              // 任务状态
            Type task_type : 2;           // FPU状态 : 任务类型 0线程 1内核进程 2 用户进程
            FpuUsed fpu_used : 1;         // FPU状态 : 1使用  0未使用
            FpuEnable fpu_enable : 1;     // FPU状态 : 1开  0未开
            uint64_t red : 53;
        } flags;

        uint64_t virtual_deadline;     // 虚拟截止时间
        uint64_t cpu_id;               // CPU ID

        Lib::ListTable thread_queue;     // 进程持有的线程队列
        uint64_t number_of_threads;      // 线程数量，最少为1（也就是进程本身）

        FloatPointUnit::FpuFrame *fpu_frame;
        uint64_t stack_magic;     // 用于检测栈的溢出

        explicit PCB( IN std::tuple< const char_t *, uint64_t, VOID *, State, std::pair< FpuEnable, FpuUsed >, Type > group, IN uint64_t _arg ) noexcept {
            Lib::SpinLock lock { };
            auto &[ _name, _priority, _entry, _state, _fpu, _type ] = group;
            lock.acquire( );
            // 内核栈处理
            this->kernel_stack_base = reinterpret_cast< uint64_t >( this ) + TASK_STACK_SIZE - PageAllocater::__page_size< MemoryPageType::PAGE_4K >;
            this->kernel_stack_size = PageAllocater::__page_size< MemoryPageType::PAGE_4K >;
            auto kernel_stack = this->kernel_stack_base + this->kernel_stack_size;
            kernel_stack -= sizeof( ProcessContext );
            this->context.pcontext = reinterpret_cast< ProcessContext * >( kernel_stack );
            if ( _type != Type::THREAD ) {
                this->context.pcontext->make( _entry, this->kernel_stack_base + this->kernel_stack_size );
            }
            kernel_stack -= sizeof( ThreadContext );
            this->context.tcontext = reinterpret_cast< ThreadContext * >( kernel_stack );
            if ( _type == Type::THREAD ) {
                this->context.tcontext->make( _entry, _arg );
            }
            else {
                // this->context.tcontext->make( (VOID *)Architecture::ArchitectureManager< TARGET_ARCH >::to_process, (uint64_t)this->context.pcontext );
            }
            // 用户栈
            if ( _type == Type::USER_PROCESS ) {
                PageTableWalker { }.copy( this->memory_manager.page_table );     // 制作页表
                this->user_stack_base = (uint64_t)PageAllocater { }.allocate< MemoryPageType::PAGE_2M >( 4 );
                this->user_stack_size = PageAllocater::__page_size< MemoryPageType::PAGE_2M > * 4;
                *(uint64_t *)physical_to_virtual( this->user_stack_base ) = uint64_t( this );
                PageTableWalker { }.map( this->user_stack_base,
                                         USER_STACK_VIRTUAL_ADDRESS_TOP - this->user_stack_size,
                                         4,
                                         PAGE_PRESENT | PAGE_RW_W | PAGE_US_U,
                                         MemoryPageType::PAGE_2M,
                                         this->memory_manager.page_table );
                this->context.pcontext->rsp = USER_STACK_VIRTUAL_ADDRESS_TOP;
            }
            // SSE
            this->fpu_frame = reinterpret_cast< FloatPointUnit::FpuFrame * >( this + 1 );
            // 分配PID
            this->PID = pid_pool.allocate( );
            // 设置进程名
            std::strncpy( this->name, _name, TASK_NAME_SIZE );

            // 暂时没啥用
            this->signal = 0;
            // 时间片越多优先级越高
            this->priority = _priority;
            // 标注，例如进程还是线程，内核级别还是用户级别，FPU的情况等
            this->flags.fpu_enable = std::get< FpuEnable >( _fpu );
            this->flags.fpu_used = std::get< FpuUsed >( _fpu );
            this->flags.state = _state;
            this->flags.task_type = _type;
#ifdef APIC
            // 当前cpu的id
            this->cpu_id = Interrupt::apic_id( );
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
            GlobalSegmentDescriptorTable::gdt->tss[ 0 ].set_rsp0( this->kernel_stack_base + this->kernel_stack_size );

#elif defined( __aarch64__ )
#else
#error Not any registers
#endif
            PageTableWalker { }.activate( this->memory_manager.page_table );
            lock.release( );
        }
    };

    inline auto get_current( VOID ) {
        pml4t page_table { (pml4t_entry *)CPU::get_page_table( ) };
        auto rsp = CPU::get_rsp( );
        rsp = (uint64_t)PageTableWalker { }.VTP_from( (VOID *)rsp, MemoryPageType::PAGE_2M, page_table );
        return (PCB *)( *(uint64_t *)rsp );
    }

    using Process = uint64_t;
    template < typename T >
    inline auto get_current( VOID ) {
        auto rsp = CPU::get_rsp( );
        return (PCB *)( *(uint64_t *)rsp );
    }
}