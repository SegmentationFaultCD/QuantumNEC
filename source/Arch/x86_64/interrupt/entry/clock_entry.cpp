#include <Arch/x86_64/platform/platform.hpp>
#include <Lib/IO/Stream/iostream>
#include <Kernel/task.hpp>
PUBLIC namespace QuantumNEC::Architecture {
    _C_LINK auto save_current_frame( IN CONST InterruptDescriptorTable::InterruptFrame * frame )->VOID;
    PRIVATE auto ASMCALL clock_handler( IN Lib::Types::Ptr< CONST Architecture::InterruptDescriptorTable::InterruptFrame > frame )->Lib::Types::Ptr< CONST Architecture::InterruptDescriptorTable::InterruptFrame > {
        using namespace Kernel;
        Architecture::Interrupt::eoi( frame->vector );
        auto current { Kernel::get_current< Process >( ) };
        save_current_frame( frame );
        if ( !current->ticks ) {
            current->schedule( );
            current->ticks = current->priority;
            return Task::ready_task->cpu_frame;
        }
        else {
            current->ticks--;
            return frame;
        }
        return frame;
    }
    ClockEntry::ClockEntry( VOID ) noexcept {
        Architecture::InterruptDescriptorTable::set_interrupt_handler( IRQ_CLOCK, clock_handler );     // 注册时间中断入口函数
    }
}