#include <kernel/cpu/cpu.hpp>
#include <kernel/print.hpp>
#include <kernel/syscall/syscall.hpp>
#include <libcxx/string.hpp>

namespace QuantumNEC::Kernel {
using namespace Kernel;
using namespace std;

/*
 * 所有syscall_开头的系统调用集合处
 */

_C_LINK auto system_call( void ) -> void;

Syscall::Syscall( void ) noexcept {
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

}     // namespace QuantumNEC::Kernel