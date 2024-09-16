#pragma once
#include <Lib/Types/Uefi.hpp>
#include <Kernel/Task/process.hpp>
#include <Lib/Debug/assert.hpp>
#include <atomic>
PUBLIC namespace QuantumNEC::Kernel {
    PUBLIC struct ThreadFrame     // 栈底
    {
#if defined( __x86_64__ )
        // ABI要求保护的寄存器
        Lib::Types::uint64_t r15;
        Lib::Types::uint64_t r14;
        Lib::Types::uint64_t r13;
        Lib::Types::uint64_t r12;
        Lib::Types::uint64_t rbx;
        Lib::Types::uint64_t rdi;
        Lib::Types::uint64_t rsi;
        Lib::Types::uint64_t rbp;
        Lib::Types::Ptr< VOID > rip;
        // 栈顶
#elif defined( __aarch64__ )
#else
#error Not any registers
#endif
        auto make( IN TaskFunction _entry, IN TaskArg _arg, IN Lib::Types::int64_t flags ) -> Lib::Types::BOOL;
        // 栈顶
    } _packed;
    PUBLIC class Thread : public TaskPCB< ThreadFrame >     // 任务管理
    {
    public:
        explicit Thread( IN CONST Lib::Types::char_t *_name, IN Lib::Types::int64_t _priority, IN Lib::Types::int64_t _flags, IN TaskFunction _entry, IN TaskArg _arg ) noexcept :
            TaskPCB< ThreadFrame > {
                _name, _priority, _flags, _entry, _arg
            } {
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
        auto unblock( IN Lib::Types::Ptr< Thread > pcb ) -> VOID;

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

    class TaskLock : std::atomic_flag
    {
    public:
        explicit TaskLock( VOID ) noexcept {
        }
        explicit TaskLock( IN Lib::Types::BOOL locked ) noexcept {
            this->_M_i = locked;
        }
        virtual ~TaskLock( VOID ) noexcept = default;

    public:
        /**
         * @brief 释放锁
         */
        auto release( VOID ) -> VOID;
        /**
         * @brief 获取锁
         */
        auto acquire( VOID ) -> VOID;
        bool locked( ) {
            return this->_M_i;
        }
    };
}