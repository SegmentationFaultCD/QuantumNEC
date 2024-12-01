#include <kernel/global/arch/x86_64/global.hpp>
#include <kernel/interrupt/arch/x86_64/entry/entry_smp_switch_cpu.hpp>
#include <kernel/interrupt/arch/x86_64/entry/idt.hpp>
#include <kernel/interrupt/arch/x86_64/pic/pic.hpp>
#include <kernel/print.hpp>
#include <kernel/task/task.hpp>
using namespace QuantumNEC;
PUBLIC namespace QuantumNEC::Kernel::x86_64 {
    Scheduler scheduler { };

    auto SymmetricMultiprocessingSwitchCPUEntry::name( VOID ) noexcept -> VOID {
    }
    auto SymmetricMultiprocessingSwitchCPUEntry::error_code( [[maybe_unused]] uint64_t error_code ) noexcept -> VOID {
    }
    auto SymmetricMultiprocessingSwitchCPUEntry::handler( Frame * frame ) noexcept -> Frame * {
        Apic::eoi( frame->vector );
        return frame;
    }
    auto SymmetricMultiprocessingSwitchCPUEntry::do_register( VOID ) -> VOID {
    }
}