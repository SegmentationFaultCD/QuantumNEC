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

auto         syscall_send_receive( IN Interrupt::InterruptFrame *frame ) -> Interrupt::InterruptFrame *;
_C_LINK auto system_call( void ) -> void;

Syscall::Syscall( void ) noexcept {
    this->set_syscall_table( static_cast< uint64_t >( Syscall::Function::SYSCALL_MESSAGE_SEND ), syscall_send_receive );
    this->set_syscall_table( static_cast< uint64_t >( Syscall::Function::SYSCALL_MESSAGE_RECEIVE ), syscall_send_receive );
    this->set_syscall_table( static_cast< uint64_t >( Syscall::Function::SYSCALL_MESSAGE_SEND_RECEIVE ), syscall_send_receive );
    this->initializate( );
    println< print_level::OK >( "Initialize system call." );
}
auto Syscall::initializate( void ) noexcept -> void {
#if defined( __x86_64__ )
    CPU::wrmsr( x86_64::IA32_EFER, CPU::rdmsr( x86_64::IA32_EFER ) | x86_64::IA32_EFER_SCE );
    CPU::wrmsr( x86_64::IA32_LSTAR, (uint64_t)system_call );
    // In Long Mode, userland CS will be loaded from STAR 63:48 + 16 and userland SS from STAR 63:48 + 8 on SYSRET
    CPU::wrmsr( x86_64::IA32_STAR, (uint64_t)x86_64::SELECTOR_CODE64_KERNEL << 32 | (uint64_t)( x86_64::SELECTOR_CODE64_USER - 16 ) << 48 );
    CPU::wrmsr( x86_64::IA32_FMASK, x86_64::RFLAGS_IF_1 );
#endif
}
auto Syscall::get_syscall_table( void ) -> SyscallEntry * {
    return syscall_table;
}
auto Syscall::set_syscall_table( IN uint64_t index, SyscallEntry entry ) -> void {
    syscall_table[ index ] = entry;
}
}     // namespace QuantumNEC::Kernel