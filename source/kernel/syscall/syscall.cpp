#include <kernel/display/print.hpp>
#include <kernel/driver/cpu/io.hpp>
#include <kernel/interrupt/apic.hpp>
#include <kernel/interrupt/entry.hpp>
#include <kernel/interrupt/idt.hpp>
#include <kernel/memory/segment/gdt.hpp>
#include <kernel/syscall/syscall.hpp>
#include <kernel/task/schedule/scheduler.hpp>
#include <kernel/task/task.hpp>
namespace Kernel {
extern "C" auto system_call_entry( void ) -> void;

class SyscallHandle : public Interrupt::GeneralInterruptHandle {
    virtual auto name( std::uint64_t ) noexcept -> void override {
    }
    virtual auto registers( Interrupt::IDT::Frame *frame ) noexcept -> void override {
    }
    virtual auto error_code( std::uint64_t code ) noexcept -> void override {
    }
    virtual auto task( ) noexcept -> void override {
    }
    virtual auto cpu( ) noexcept -> void override {
    }
    virtual auto handler( Interrupt::IDT::Frame *frame ) noexcept -> Interrupt::IDT::Frame * override {
        Interrupt::apic.eoi( );
        syscall->call( frame );
        return frame;
    }

} handler;
auto Syscall::call( Interrupt::IDT::Frame *frame ) -> Interrupt::IDT::Frame * {
    if ( this->table[ frame->regs.rax ] ) {
        return this->table[ frame->regs.rax ]->handle( frame );
    }
    else {
        return frame;
    }
}

extern "C" auto system_call( Interrupt::IDT::Frame *frame ) -> Interrupt::IDT::Frame * {
    Display::println( "Syscall" );
    // Task::scheduler->yield( );
    return frame;
}
auto Syscall::initialize( void ) -> Syscall * {
    static Syscall syscall { };
    using namespace Driver;
    IO::wrmsr( IO::IA32_EFER, IO::rdmsr( IO::IA32_EFER ) | IO::IA32_EFER_SCE );
    IO::wrmsr( IO::IA32_LSTAR, (uint64_t)system_call_entry );
    // In Long Mode, userland CS will be loaded from STAR 63:48 + 16 and userland SS from STAR 63:48 + 8 on SYSRET
    IO::wrmsr( IO::IA32_STAR, (uint64_t)Memory::GDT::SELECTOR_CODE64_KERNEL << 32 | (uint64_t)( Memory::GDT::SELECTOR_CODE64_USER - 16 ) << 48 );
    IO::wrmsr( IO::IA32_FMASK, 1 << 9 );

    Interrupt::GeneralInterruptHandle::register_handle( Interrupt::IDT::SYSCALL, &handler );
    Interrupt::apic.register_ioapic( Interrupt::IDT::SYSCALL, 96 );

    return &syscall;
}
}     // namespace Kernel