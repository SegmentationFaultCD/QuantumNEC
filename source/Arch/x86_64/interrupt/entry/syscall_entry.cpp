#include "Arch/x86_64/platform/syscall.hpp"
#include "Kernel/Task/process.hpp"
#include <Arch/x86_64/platform/platform.hpp>
#include <Kernel/task.hpp>

PUBLIC namespace QuantumNEC::Architecture {
    _C_LINK auto save_current_frame( IN CONST InterruptDescriptorTable::InterruptFrame * frame )->VOID;
    _C_LINK auto syscall_entry( IN Lib::Types::Ptr< CONST InterruptDescriptorTable::InterruptFrame > frame )->IN Lib::Types::Ptr< CONST InterruptDescriptorTable::InterruptFrame > {
        using Kernel::Task;
        save_current_frame( frame );
        auto entry = Syscall::get_syscall_table( )[ frame->regs.rax ];

        if ( !entry ) {
            const_cast< InterruptDescriptorTable::InterruptFrame * >( frame )->regs.rax = static_cast< Lib::Types::uint64_t >( Syscall::SyscallStatus::NO_SYSCALL );
            return frame;
        }
        return entry( const_cast< InterruptDescriptorTable::InterruptFrame * >( frame ) );
    }

    SystemcallEntry::SystemcallEntry( VOID ) noexcept {
        Architecture::InterruptDescriptorTable::set_interrupt_handler( IRQ_SYSTEM_CALL, syscall_entry );     // 注册系统调用中断入口函数
    }
}
