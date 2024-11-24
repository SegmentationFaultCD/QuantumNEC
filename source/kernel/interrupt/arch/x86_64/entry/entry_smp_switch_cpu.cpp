#include <kernel/global/arch/x86_64/global.hpp>
#include <kernel/interrupt/arch/x86_64/entry/entry.hpp>
#include <kernel/interrupt/arch/x86_64/entry/idt.hpp>
#include <kernel/interrupt/arch/x86_64/pic/pic.hpp>
#include <kernel/print.hpp>
#include <kernel/task/task.hpp>
using namespace QuantumNEC;
PUBLIC namespace QuantumNEC::Kernel::x86_64 {
    PRIVATE auto smp_switch_cpu _asmcall( IN CONST InterruptDescriptorTable::InterruptFrame * frame, IN uint64_t ) -> CONST InterruptDescriptorTable::InterruptFrame * {
        // 在这里进行任务调度
        
        return frame;
    }

    SymmetricMultiprocessingSwitchCPUEntry::SymmetricMultiprocessingSwitchCPUEntry( VOID ) noexcept {
        InterruptDescriptorTable::register_IPI( IRQ_SMP_SWITCH_CPU, NULL, smp_switch_cpu, 0, "SMP switch CPU", NULL );
    }
}