#include <kernel/driver/cpu/io.hpp>
#include <kernel/memory/segment/gdt.hpp>
#include <kernel/syscall/syscall.hpp>
namespace Kernel {
extern "C" auto system_call( void ) -> void;

auto Syscall::initialize( void ) -> void {
    using namespace Driver;
    IO::wrmsr( IO::IA32_EFER, IO::rdmsr( IO::IA32_EFER ) | IO::IA32_EFER_SCE );
    IO::wrmsr( IO::IA32_LSTAR, (uint64_t)system_call );
    // In Long Mode, userland CS will be loaded from STAR 63:48 + 16 and userland SS from STAR 63:48 + 8 on SYSRET
    IO::wrmsr( IO::IA32_STAR, (uint64_t)Memory::GDT::SELECTOR_CODE64_KERNEL << 32 | (uint64_t)( Memory::GDT::SELECTOR_CODE64_USER - 16 ) << 48 );
    IO::wrmsr( IO::IA32_FMASK, 1 << 9 );
}
}     // namespace Kernel