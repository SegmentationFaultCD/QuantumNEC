#pragma once
#include <Lib/Types/Uefi.hpp>
#include <Lib/Types/type_bool.hpp>
#include <Arch/Arch.hpp>
#include <Kernel/memory.hpp>
#include <Kernel/Task/message.hpp>
#include <Lib/Base/allocate.hpp>
#include <Lib/Base/attribute.hpp>
#include <Lib/STL/list>
#include <Lib/STL/string>
#include <Lib/IO/Stream/iostream>
#include <Kernel/Task/pid.hpp>
#include <concepts>
PUBLIC namespace QuantumNEC::Kernel {
    PUBLIC constexpr CONST auto TASK_STACK_SIZE { 65536ull };     // 64KB
    PUBLIC constexpr CONST auto TASK_STACK_MAGIC { 0x1145141919810ULL };
    PUBLIC constexpr CONST auto TASK_NAME_SIZE { 32ull };
    PUBLIC constexpr CONST auto TASK_FLAG_THREAD { 1ull << 0 };
    PUBLIC constexpr CONST auto TASK_FLAG_KERNEL_PROCESS { 1ull << 1 };
    PUBLIC constexpr CONST auto TASK_FLAG_USER_PROCESS { 1ull << 2 };
    PUBLIC constexpr CONST auto TASK_FLAG_FPU_USED { 1ull << 3 };
    PUBLIC constexpr CONST auto TASK_FLAG_FPU_UNUSED { 1ull << 4 };
    PUBLIC constexpr CONST auto TASK_FLAG_FPU_ENABLE { 1ull << 5 };

    PUBLIC using TaskArg = Lib::Types::uint64_t;
    PUBLIC using TaskReturnValue = Lib::Types::int64_t;
    PUBLIC using TaskFunction = Lib::Types::FuncPtr< TaskReturnValue, TaskArg >;

    PUBLIC enum class TaskStatus : Lib::Types::int64_t {
        RUNNING,
        READY,
        BLOCKED,
        SENDING,
        RECEIVING,
        WAITING,
        HANGING,
        DIED,
    };
    PUBLIC inline Lib::STL::ListTable all_task_group { };
    PUBLIC inline Lib::STL::ListTable ready_task_group { };

    PUBLIC template < typename Frame >
        requires std::is_compound_v< Frame > && requires( Frame f, IN TaskFunction _entry, IN TaskArg _arg, IN Lib::Types::int64_t _flags ) {
            { f.make( _entry, _arg, _flags ) } -> std::same_as< Lib::Types::BOOL >;
        }
    struct TaskPCB
    {
        // 连接每个任务
        Lib::STL::ListNode all_task_queue;         // 所有任务队列
        Lib::STL::ListNode general_task_queue;     // 通用任务队列
        // 记录内存分布
        Lib::Types::Ptr< Lib::Types::uint64_t > page_directory;     // 任务所持有的页表地址
        Lib::Base::Allocate allocate_table;                         // 虚拟地址表，在page_directory不为空时有效
        // 记录寄存器状态
        volatile Lib::Types::Ptr< Frame > cpu_frame;                                                          // 保存的CPU寄存器状态信息
        volatile Lib::Types::Ptr< Architecture::ArchitectureManager< TARGET_ARCH >::FPUFrame > fpu_frame;     // 保存的FPU寄存器状态信息
        // 进程间通信需要的
        Message message;     // 进程消息体
        // 任务信息
        Lib::Types::uint64_t PID;                      // 任务ID
        Lib::Types::char_t name[ TASK_NAME_SIZE ];     // 任务名
        volatile TaskStatus status;                    // 任务状态
        Lib::Types::int64_t counter;                   // 任务总时间片
        Lib::Types::int64_t ticks;                     // 每次在处理器上执行的时间嘀嗒数
        Lib::Types::int64_t signal;                    // 任务持有的信号
        Lib::Types::int64_t priority;                  // 任务优先级
        Lib::Types::int64_t flags;                     // 任务特殊标志
        Lib::Types::uint64_t stack_magic;              // 用于检测栈的溢出

        explicit TaskPCB( IN CONST Lib::Types::char_t *_name, IN Lib::Types::int64_t _priority, IN Lib::Types::int64_t _flags, IN TaskFunction _entry, IN TaskArg _arg ) noexcept {
            // 将任务pcb添加到队列
            this->all_task_queue.container = this;
            this->general_task_queue.container = this;
            Lib::STL::list_add_to_end( &ready_task_group, &this->general_task_queue );
            Lib::STL::list_add_to_end( &all_task_group, &this->all_task_queue );
            /*
             * 分布如下
             * ---------------------------------------64KB-----------------------------------------
             * sizeof(TaskPCB)---cpu_frame---fpu_frame---进程栈 ( RSP 指向64KB尾巴 )
             * Task PCB        ~ 存CPU状态  ~ FPU状态    ~ 空                  ~ TOP
             */
            // 分配 cpu frame
            this->cpu_frame = reinterpret_cast< Lib::Types::Ptr< Frame > >( this + 1 );
            // 分配 fpu frame
            this->fpu_frame = reinterpret_cast< Lib::Types::Ptr< Architecture::ArchitectureManager< TARGET_ARCH >::FPUFrame > >( this->cpu_frame + 1 );
            // 设置消息
            this->message.send_to = NO_TASK;
            this->message.receive_from = NO_TASK;
            Lib::STL::list_init( &this->message.sender_list );
            // 分配PID
            this->PID = PidPool::allocate( );
            // 设置进程名
            Lib::STL::strncpy( this->name, _name, TASK_NAME_SIZE );
            // 设置进程初始状态
            this->status = TaskStatus::READY;
            // 计算这个进程使用的总时间片
            this->counter = 0;
            // 这个进程可的使用时间
            this->ticks = _priority;
            // 暂时没啥用
            this->signal = 0;
            // 时间片越多优先级越高
            this->priority = _priority;
            // 标注，例如进程还是线程，内核级别还是用户级别，FPU的情况等
            this->flags |= flags;
            // 魔术字节
            this->stack_magic = TASK_STACK_MAGIC;
            // 制作页表
            this->page_directory = ( ( _flags & TASK_FLAG_KERNEL_PROCESS ) ? NULL : ( ( _flags & TASK_FLAG_USER_PROCESS ) ? reinterpret_cast< decltype( this->page_directory ) >( Memory::memory_paging->make_page_table( ) ) : NULL ) );
            this->allocate_table = Lib::Base::Allocate { sizeof( Lib::Base::Allocate ::AllocateTableEntry ) * 1024 };
            this->allocate_table.free( USER_STACK_VIRTUAL_START_ADDRESS / PAGE_SIZE, ( USER_STACK_VIRTUAL_END_ADDRESS - USER_STACK_VIRTUAL_START_ADDRESS ) / PAGE_SIZE );
            if ( !this->page_directory && ( _flags & TASK_FLAG_USER_PROCESS ) ) {
                Lib::STL::memset( this, 0, TASK_STACK_SIZE );
                Memory::page->free( this, 1, PageMemory::MemoryPageType::PAGE_2M );
            }

            if ( !this->cpu_frame->make( _entry, _arg, _flags ) ) {
                // 如果分配失败那么就释放所有的内存并返回
                Lib::STL::memset( this->cpu_frame, 0, sizeof( Frame ) );
                Lib::STL::memset( this, 0, TASK_STACK_SIZE );
                Memory::page->free( this, 1, PageMemory::MemoryPageType::PAGE_2M );
            }
        }
        explicit TaskPCB( IN TaskPCB & ) = delete;
        explicit TaskPCB( VOID ) = delete;
        /**
         * @brief 激活任务
         * @param pcb 要激活的任务PCB
         */

        auto activate( IN Lib::Types::Ptr< TaskPCB< Frame > > pcb ) {
            Memory::memory_paging->activate_page_table( pcb->page_directory );
            if ( pcb->page_directory ) {
#if defined( __x86_64__ )
                Architecture::GlobalSegmentDescriptorTable::tss[ 0 ].set_rsp0< TASK_STACK_SIZE >( pcb );
#elif defined( __aarch64__ )
#else
#error Not any registers
#endif
            }
        }
    };

    PUBLIC struct ProcessFrame :
#if defined( __x86_64__ )
        Architecture::InterruptDescriptorTable::InterruptFrame
#elif defined( __aarch64__ )
#else
#error Not any registers
#endif
    {
        auto make( IN TaskFunction _entry, IN TaskArg _arg, IN Lib::Types::int64_t flags ) -> Lib::Types::BOOL;
    };

    PUBLIC class Process : public TaskPCB< ProcessFrame >
    {
    public:
        explicit Process( IN CONST Lib::Types::char_t *_name, IN Lib::Types::int64_t _priority, IN Lib::Types::int64_t _flags, IN TaskFunction _entry, IN TaskArg _arg ) noexcept :
            TaskPCB< ProcessFrame > { _name, _priority, _flags, _entry, _arg } {
        }

    public:
        /**
         * @brief 调度
         */
        auto schedule( VOID ) -> VOID;
        /**
         * @brief 阻塞任务
         */
        auto block( IN TaskStatus state ) -> VOID;
        /**
         * @brief 解除任务阻塞
         * @param pcb 指向的任务PCB
         */
        auto unblock( IN Lib::Types::Ptr< Process > pcb ) -> VOID;

    public:
        /**
         * @brief 查找任务
         * @param PID 要找的任务ID
         * @retval 找到返回该任务pcb地址，没有则是NULL
         */
        auto find( IN Lib::Types::uint64_t PID ) {
            Lib::Types::Ptr< Lib::STL::ListNode > node {
                Lib::STL::list_traversal(
                    &all_task_group,
                    [ & ]( Lib::Types::Ptr< Lib::STL::ListNode > node, Lib::Types::uint32_t PID ) -> Lib::Types::BOOL { return reinterpret_cast< Lib::Types::Ptr< Process > >( node->container )->PID == PID; },
                    PID )
            };
            return !node ? NULL : reinterpret_cast< Lib::Types::Ptr< Process > >( node->container );
        }
    };
    /**
     * @brief 程序控制块PCB
     * @tparam Frame PCB属于进程还是线程
     */
    PUBLIC template < typename PCB >
    inline auto get_current( VOID )
        requires std::is_compound_v< PCB > && requires( PCB *pcb, TaskStatus status ) {
            { pcb->schedule( ) } -> std::same_as< VOID >;
            { pcb->block( status ) } -> std::same_as< VOID >;
            { pcb->unblock( pcb ) } -> std::same_as< VOID >;
            { pcb->find( 0ull ) } -> std::same_as< PCB * >;
        }
    {
        return reinterpret_cast< Lib::Types::Ptr< PCB > >( Architecture::ArchitectureManager< TARGET_ARCH >::get_rsp( ) & ~( TASK_STACK_SIZE - 1 ) );
    }
}