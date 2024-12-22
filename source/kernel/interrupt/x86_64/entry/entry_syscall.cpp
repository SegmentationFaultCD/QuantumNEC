#include <kernel/cpu/cpu.hpp>
#include <kernel/global/x86_64/global.hpp>
#include <kernel/interrupt/x86_64/entry/entry_syscall.hpp>
#include <kernel/interrupt/x86_64/entry/idt.hpp>
#include <kernel/interrupt/x86_64/pic/pic.hpp>
#include <kernel/print.hpp>
#include <kernel/syscall/syscall.hpp>
using namespace QuantumNEC;
namespace QuantumNEC::Kernel::x86_64 {

_C_LINK auto system_call( void ) -> void;

auto SystemcallEntry::name( void ) noexcept -> void {
}
auto SystemcallEntry::error_code( [[maybe_unused]] uint64_t error_code ) noexcept -> void {
}
auto SystemcallEntry::handler( Frame *frame ) noexcept -> Frame * {
    Apic::eoi( frame->vector );
    auto entry = Syscall::get_syscall_table( )[ frame->regs.rax ];
    if ( !entry ) {
        frame->regs.rax = static_cast< uint64_t >( Syscall::Status::NO_SYSCALL );
        return frame;
    }

    return entry( frame );
}
auto SystemcallEntry::do_register( void ) -> void {
    Apic::IOApicRedirectionEntry entry { };
    entry.vector         = IRQ_SYSTEM_CALL;
    entry.deliver_mode   = APIC_ICR_IOAPIC_FIXED;
    entry.dest_mode      = ICR_IOAPIC_DELV_PHYSICAL;
    entry.deliver_status = APIC_ICR_IOAPIC_IDLE;
    entry.polarity       = APIC_IOAPIC_POLARITY_HIGH;
    entry.irr            = APIC_IOAPIC_IRR_RESET;
    entry.trigger        = APIC_ICR_IOAPIC_EDGE;
    entry.mask           = APIC_ICR_IOAPIC_MASKED;
    Apic::install_ioapic( IRQ_SYSTEM_CALL, &entry );
    Apic::enable_ioapic( IRQ_SYSTEM_CALL );

    auto val = CPU::rdmsr( x86_64::IA32_EFER );
    val |= x86_64::IA32_EFER_SCE;
    CPU::wrmsr( x86_64::IA32_EFER, val );
    CPU::wrmsr( x86_64::IA32_LSTAR, (uint64_t)system_call );
    // In Long Mode, userland CS will be loaded from STAR 63:48 + 16 and userland SS from STAR 63:48 + 8 on SYSRET
    CPU::wrmsr( x86_64::IA32_STAR, (uint64_t)x86_64::SELECTOR_CODE64_KERNEL << 32 | (uint64_t)( x86_64::SELECTOR_CODE64_USER - 16 ) << 48 );
    CPU::wrmsr( x86_64::IA32_FMASK, x86_64::RFLAGS_IF_1 );
}
}     // namespace QuantumNEC::Kernel::x86_64
