#include <kernel/cpu/cpu.hpp>
#include <kernel/print.hpp>
#include <kernel/syscall/syscall.hpp>
#include <libcxx/string.hpp>

QuantumNEC::Kernel::Syscall::SyscallEntry syscall_table[ QuantumNEC::Kernel::NR_SYS_CALL ] { };
namespace QuantumNEC::Kernel {
using namespace Kernel;
using namespace std;

/*
 * 所有syscall_开头的系统调用集合处
 */

auto syscall_send_receive( IN Interrupt::InterruptFrame *frame ) -> Interrupt::InterruptFrame *;

Syscall::Syscall( void ) noexcept {
    this->set_syscall_table( static_cast< uint64_t >( Syscall::Function::SYSCALL_MESSAGE_SEND ), syscall_send_receive );
    this->set_syscall_table( static_cast< uint64_t >( Syscall::Function::SYSCALL_MESSAGE_RECEIVE ), syscall_send_receive );
    this->set_syscall_table( static_cast< uint64_t >( Syscall::Function::SYSCALL_MESSAGE_SEND_RECEIVE ), syscall_send_receive );
    println< print_level::OK >( "Initialize system call." );
}
auto Syscall::get_syscall_table( void ) -> SyscallEntry * {
    return syscall_table;
}
auto Syscall::set_syscall_table( IN uint64_t index, SyscallEntry entry ) -> void {
    syscall_table[ index ] = entry;
}
}     // namespace QuantumNEC::Kernel