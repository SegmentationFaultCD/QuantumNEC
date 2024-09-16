#include <Kernel/task.hpp>
#include <Arch/Arch.hpp>
#include <Lib/IO/Stream/iostream>
#include <Lib/STL/string>

PUBLIC namespace {
    using namespace QuantumNEC::Kernel;

    TaskLock lock { };
}
PUBLIC namespace QuantumNEC::Kernel {
    auto ThreadFrame::make( IN TaskFunction _entry, IN TaskArg _arg, IN Lib::Types::int64_t _flags )->Lib::Types::BOOL {
        if ( _flags & TASK_FLAG_THREAD ) {
#if defined( __x86_64__ )
            // 没啥好解释的。。。
            this->rdi = _arg;
            this->rsi = 0;
            this->rip = reinterpret_cast< decltype( this->rip ) >( _entry );
            this->rbp = 0;
            this->rbx = 0;
            this->r15 = 0;
            this->r14 = 0;
            this->r13 = 0;
            this->r12 = 0;
#elif defined( __aarch64__ )
#else
#error There are no instructions available to set the registers
#endif
            return TRUE;
        }
        else {
            return FALSE;
        }
    }
    auto Thread::schedule( VOID )->VOID {
        return;
    }
    auto Thread::block( IN TaskStatus state )->VOID {
        auto interrupt_status { Architecture::ArchitectureManager< TARGET_ARCH >::disable_interrupt( ) };
        this->status = state;
        this->schedule( );
        Architecture::ArchitectureManager< TARGET_ARCH >::set_interrupt( interrupt_status );
        return;
    }
    auto Thread::unblock( IN Lib::Types::Ptr< Thread > pcb )->VOID {
        if ( !pcb ) {
            return;
        }
        auto interrupt_status { Architecture::ArchitectureManager< TARGET_ARCH >::disable_interrupt( ) };
        if ( pcb->status != TaskStatus::READY ) {
            // 放到队列的最前面，使其尽快得到调度
            Lib::STL::list_add_to_head( &ready_task_group, &pcb->general_task_queue );
            pcb->status = TaskStatus::READY;
        };
        Architecture::ArchitectureManager< TARGET_ARCH >::set_interrupt( interrupt_status );
        return;
    }
    auto TaskLock::release( VOID )->VOID {
        std::atomic_flag_clear_explicit( this, std::memory_order_release );
    }
    auto TaskLock::acquire( VOID )->VOID {
        while ( std::atomic_flag_test_and_set_explicit( this, std::memory_order_acquire ) ) {
            Architecture::ArchitectureManager< TARGET_ARCH >::pause( );
        }
    }
}