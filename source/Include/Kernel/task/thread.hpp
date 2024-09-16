#pragma once
#include <Lib/Uefi.hpp>
#include <Kernel/task/process.hpp>

PUBLIC namespace QuantumNEC::Kernel {
    PUBLIC class Thread : public TaskPCB< ThreadFrame >     // 任务管理
    {
    public:
        explicit Thread( IN CONST char_t *_name, IN int64_t _priority, IN int64_t _flags, IN TaskFunction _entry, IN TaskArg _arg ) noexcept :
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
         */
        auto unblock( VOID ) -> VOID;

    public:
        /**
         * @brief 查找任务
         * @param PID 要找的任务ID
         * @retval 找到返回该任务pcb地址，没有则是NULL
         */
        auto find( IN uint64_t PID ) {
            Lib::SpinLock lock { };
            lock.acquire( );
            Ptr< Lib::ListNode > node {
                all_task_group.traversal(
                    [ & ]( Lib::ListNode *node, uint32_t PID ) -> BOOL { return reinterpret_cast< Thread * >( node->container )->PID == PID; },
                    PID )
            };
            lock.release( );
            return !node ? NULL : reinterpret_cast< Thread * >( node->container );
        }
    };
}