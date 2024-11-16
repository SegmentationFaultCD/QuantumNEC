#pragma once
#include <kernel/cpu/cpu.hpp>
#include <kernel/driver/driver.hpp>
#include <kernel/memory/memory.hpp>
#include <kernel/print.hpp>
#include <kernel/syscall/message.hpp>
#include <kernel/task/general/pcb/pid.hpp>
#include <lib/Uefi.hpp>
#include <lib/list.hpp>
#include <lib/spin_lock.hpp>
#include <libcxx/cstring.hpp>

PUBLIC namespace QuantumNEC::Kernel {
    PUBLIC constexpr CONST auto TASK_STACK_SIZE { 4_KB };     // 64KB
    PUBLIC constexpr CONST auto TASK_STACK_MAGIC { 0x1145141919810ULL };
    PUBLIC constexpr CONST auto TASK_NAME_SIZE { 32ull };

    PUBLIC struct PCB {
    public:
        struct MM {
            pml4t page_table;     // 任务所持有的页表地址
            explicit MM( VOID ) = default;
            auto operator=( MM &mm ) -> MM & {
                this->page_table = mm.page_table;
                return *this;
            }
        };
        struct _packed ThreadContext     // 栈底
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
            RFlags   rflags;
            VOID    *rip;
            // 栈顶
#elif defined( __aarch64__ )
#else
#error Not any registers
#endif
            auto make( IN VOID *_entry, IN uint64_t _arg ) -> BOOL {
            }
            // 栈顶
        };
        struct ProcessContext : Interrupt::InterruptFrame {
            auto make( IN VOID *_entry, IN uint64_t kernel_stack_top ) -> BOOL {
            }
        };
        struct Context {
            ProcessContext *pcontext;
            ThreadContext  *tcontext;
            explicit Context( VOID ) = default;
        };

    public:
        enum class State : uint64_t {
            RUNNING   = 0,
            READY     = 1,
            BLOCKED   = 2,
            SENDING   = 3,
            RECEIVING = 4,
            WAITING   = 5,
            HANGING   = 6,
            DIED      = 7,
        };
        enum class FpuEnable : uint64_t {
            ENABLE  = 1,
            DISABLE = 0
        };
        enum class FpuUsed : uint64_t {
            USED   = 1,
            UNUSED = 0,
        };
        enum class Type : uint64_t {
            THREAD         = 0,
            KERNEL_PROCESS = 1,
            USER_PROCESS   = 2
        };

    public:
        Lib::ListNode general_task_node;     // 通用任务队列 连接除running状态的每个任务

        MM memory_manager;     // 记录内存分布

        uint64_t kernel_stack_base;     // 内核栈栈底
        uint64_t kernel_stack_size;     // 内核栈大小
        uint64_t user_stack_base;       // 用户栈栈底
        uint64_t user_stack_size;       // 用户栈大小
        Context  context;               // 上下文 记录寄存器状态

        Message message;     // 进程消息体

        uint64_t PID;                        // 任务ID
        uint64_t PPID;                       // 父进程ID
        char_t   name[ TASK_NAME_SIZE ];     // 任务名

        uint64_t jiffies;      // 可运行的时间片
        uint64_t signal;       // 任务持有的信号
        uint64_t priority;     // 任务优先级

        struct Flags {
            State     state : 7;          // 任务状态
            Type      task_type : 2;      // FPU状态 : 任务类型 0线程 1内核进程 2 用户进程
            FpuUsed   fpu_used : 1;       // FPU状态 : 1使用  0未使用
            FpuEnable fpu_enable : 1;     // FPU状态 : 1开  0未开
            uint64_t  red : 53;
        } flags;

        uint64_t virtual_deadline;     // 虚拟截止时间
        uint64_t cpu_id;               // CPU ID

        FloatPointUnit::FpuFrame *fpu_frame;

        uint64_t stack_magic;     // 用于检测栈的溢出

    public:
        explicit PCB( CONST char_t *_name_, uint64_t _priority_, Flags _flags_, VOID *_entry_, IN uint64_t _arg_ ) noexcept;

        /**
         * @brief 激活任务
         * @param pcb 要激活的任务PCB
         */

        auto activate( VOID ) {
#if defined( __x86_64__ )
            // 设置TSS，将TSS设置指向内核栈栈顶
            GlobalSegmentDescriptorTable::gdt->tss[ 0 ].set_rsp0( this->kernel_stack_base + this->kernel_stack_size );
#elif defined( __aarch64__ )
#else
#error Not any registers
#endif
            // 激活页表
            PageTableWalker { }.activate( this->memory_manager.page_table );
        }
    };
    inline auto get_current( VOID ) -> PCB * {
    }
}