#pragma once
#include <kernel/cpu/cpu.hpp>
#include <kernel/driver/driver.hpp>
#include <kernel/memory/memory.hpp>
#include <kernel/print.hpp>
#include <kernel/task/general/pcb/pid.hpp>

#include <kernel/task/general/scheduler/scheduler.hpp>
#include <lib/Uefi.hpp>
#include <lib/list.hpp>
#include <lib/spin_lock.hpp>
#include <libcxx/cstring.hpp>
namespace QuantumNEC::Kernel {

constexpr auto TASK_KERNEL_STACK_SIZE { 4_KB };     // 4KB
constexpr auto TASK_USER_STACK_SIZE { 8_MB };       // 8MB
constexpr auto PCB_STACK_MAGIC { 0x1145141919810ULL };
constexpr auto TASK_NAME_SIZE { 64 };

struct PCB {
    struct Flags {
        enum class Fpu : uint64_t {
            ENABLE  = 1,
            DISABLE = 0,
            USED    = 1,
            UNUSED  = 0,
        };
        Fpu fpu_enable : 1;     // FPU状态 : 1开  0未开
        Fpu fpu_used : 1;       // FPU状态 : 1使用  0未使用

        enum class Type : uint64_t {
            THREAD         = 0,
            KERNEL_PROCESS = 1,
            USER_PROCESS   = 2
        } task_type : 2;     // 任务类型 0线程 1内核进程 2 用户进程

        uint64_t : 53;
    } flags;

    struct _packed ThreadContext     // 栈底
    {
#if defined( __x86_64__ )
        // ABI要求保护的寄存器
        uint64_t       r15;
        uint64_t       r14;
        uint64_t       r13;
        uint64_t       r12;
        uint64_t       rbp;
        uint64_t       rbx;
        uint64_t       rdi;
        uint64_t       rsi;
        x86_64::RFlags rflags;
        void          *rip;
        // 栈顶
#elif defined( __aarch64__ )
#else
#error Not any registers
#endif
        auto make( IN void *_entry, IN uint64_t _arg ) -> bool;
        // 栈顶
    };
    struct _packed ProcessContext : Interrupt::InterruptFrame {
        ProcessContext( void ) = default;
        auto make( IN void *_entry, IN uint64_t kernel_stack_top, IN Flags::Type type ) -> bool;
    };

public:
    struct MemoryManager {
        // 任务所持有的页表地址
#if defined( __x86_64__ )
    public:
        x86_64::pml4t page_table;

#elif defined( __aarch64__ )
#endif
        struct MemoryMap {
            uint64_t text_start;
            uint64_t text_end;
            uint64_t data_start;
            uint64_t data_end;
            MemoryMap( void ) {
            }
        } map;

        MemoryManager( void ) :
            page_table { }, map { } {
        }
        ~MemoryManager( void ) {
        }
    } memory_manager;     // 记录内存分布

    uint64_t kernel_stack_base;     // 内核栈栈底
    uint64_t kernel_stack_size;     // 内核栈大小
    uint64_t user_stack_base;       // 用户栈栈底
    uint64_t user_stack_size;       // 用户栈大小

    struct Context {
        ProcessContext *pcontext;
        ThreadContext  *tcontext;
    } context;     // 上下文 记录寄存器状态

    uint64_t PID;                        // 任务ID
    uint64_t PPID;                       // 父进程ID
    char_t   name[ TASK_NAME_SIZE ];     // 任务名

    Scheduler::Schedule schedule;

    FloatPointUnit::FpuFrame *fpu_frame;

    uint64_t stack_magic;     // 用于检测栈的溢出
    explicit PCB( void ) noexcept {
    }
    explicit PCB( const char_t *_name_, uint64_t _priority_, Flags _flags_, void *_entry_, IN uint64_t _arg_ ) noexcept;

    ~PCB( void ) noexcept;

    /**
     * @brief 激活任务
     * @param pcb 要激活的任务PCB
     */

    auto activate( void ) {
#if defined( __x86_64__ )
        // 设置TSS，将TSS设置指向内核栈栈顶
        x86_64::GlobalSegmentDescriptorTable::gdt->tss[ this->schedule.cpu_id ].set_rsp0( (uint64_t)physical_to_virtual( this->kernel_stack_base ) + this->kernel_stack_size );

#elif defined( __aarch64__ )
#else
#error Not any registers
#endif
        // activate the fpu
        // this->fpu_frame->load( );

        // activate the page table
        this->memory_manager.page_table.activate( );
        if ( this->flags.task_type == Flags::Type::USER_PROCESS ) {
            // CPU::wrmsr( x86_64::IA32_KERNEL_GS_BASE, x86_64::GlobalSegmentDescriptorTable::gdt->tss[ this->schedule.cpu_id ].get_rsp0( ) );
            // CPU::wrmsr( x86_64::IA32_USER_GS_BASE, this->context.pcontext->rsp );
        }
        // set running state
    }
    /**
     * @brief remove task from running state
     */
    auto close( ) {
        // this->fpu_frame->save( );
    }
    static auto get_running_task( void ) {
        // 得到正在运行的任务
        auto kstack = CPU::get_rsp( ) & ~( TASK_KERNEL_STACK_SIZE - 1 );
        return (PCB *)( *( (uint64_t *)kstack ) );
    }
};
}     // namespace QuantumNEC::Kernel