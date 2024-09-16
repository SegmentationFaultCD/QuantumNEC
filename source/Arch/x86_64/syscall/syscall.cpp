#include <Arch/x86_64/platform/platform.hpp>
#include <Kernel/print.hpp>
#include <Kernel/kernel.hpp>
#include <Libcxx/string.hpp>

PUBLIC QuantumNEC::Architecture::Syscall::SyscallEntry syscall_table[ QuantumNEC::Architecture::NR_SYS_CALL ] { };
PUBLIC namespace QuantumNEC::Architecture {
    using namespace Kernel;
    using namespace Lib;

    /*
     * 所有syscall_开头的系统调用集合处
     */

    PUBLIC auto syscall_send_receive( IN InterruptDescriptorTable::InterruptFrame * frame )->InterruptDescriptorTable::InterruptFrame *;

    Syscall::Syscall( VOID ) noexcept {
        this->set_syscall_table( static_cast< uint64_t >( SyscallFunction::SYSCALL_MESSAGE_SEND ), syscall_send_receive );
        this->set_syscall_table( static_cast< uint64_t >( SyscallFunction::SYSCALL_MESSAGE_RECEIVE ), syscall_send_receive );
        this->set_syscall_table( static_cast< uint64_t >( SyscallFunction::SYSCALL_MESSAGE_SEND_RECEIVE ), syscall_send_receive );
        println< ostream::HeadLevel::OK >( "Initialize the system call." );
    }
    auto Syscall::get_syscall_table( VOID )->SyscallEntry * {
        return syscall_table;
    }
    auto Syscall::set_syscall_table( IN uint64_t index, SyscallEntry entry )->VOID {
        syscall_table[ index ] = entry;
    }
}