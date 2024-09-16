#pragma once
#include <Kernel/task/pcb.hpp>
#include <Kernel/task/thread.hpp>
PUBLIC namespace QuantumNEC::Kernel {
    PUBLIC struct ProcessFrame
    {
#if defined( __x86_64__ )
        Architecture::InterruptDescriptorTable::InterruptFrame iframe;
        ThreadFrame *tframe;
#elif defined( __aarch64__ )
#else
#error Not any registers
#endif

        auto make( IN TaskFunction _entry, IN TaskArg _arg, IN int64_t flags ) -> BOOL;
    };
    PUBLIC class Process : public TaskPCB< ProcessFrame >
    {
    public:
        explicit Process( IN CONST char_t *_name, IN int64_t _priority, IN int64_t _flags, IN TaskFunction _entry, IN TaskArg _arg ) noexcept :
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
            auto node {
                all_task_group.traversal(
                    [ & ]( Lib::ListNode *node, uint32_t PID ) -> BOOL { return reinterpret_cast< Process * >( node->container )->PID == PID; },
                    PID )
            };
            lock.release( );
            return !node ? NULL : reinterpret_cast< Process * >( node->container );
        }
    };
    /**
     * @brief 程序控制块PCB
     * @tparam Frame PCB属于进程还是线程
     */
    PUBLIC template < typename PCB >
    inline auto get_current( VOID ) {
        return reinterpret_cast< PCB * >( Architecture::ArchitectureManager< TARGET_ARCH >::get_rsp( ) & ~( TASK_STACK_SIZE - 1 ) );
    }
}