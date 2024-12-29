#include <kernel/global/x86_64/global.hpp>
#include <kernel/interrupt/x86_64/entry/entry_smp_switch_cpu.hpp>
#include <kernel/interrupt/x86_64/entry/idt.hpp>
#include <kernel/interrupt/x86_64/pic/pic.hpp>
#include <kernel/print.hpp>
#include <kernel/task/task.hpp>
using namespace QuantumNEC;
namespace QuantumNEC::Kernel::x86_64 {
Scheduler scheduler { };

auto SymmetricMultiprocessingSwitchCPUEntry::name( void ) noexcept -> void {
}
auto SymmetricMultiprocessingSwitchCPUEntry::error_code( [[maybe_unused]] uint64_t error_code ) noexcept -> void {
}
auto SymmetricMultiprocessingSwitchCPUEntry::handler( Frame *frame ) noexcept -> Frame * {
    Apic::eoi( frame->vector );

    // auto result = Scheduler { }.schedule( );
    // if ( result.has_value( ) ) {
    //     if ( result.value( )->general_task_node.container == ProcessManager::main_pcb ) {
    //         return frame;
    //     }
    //     else {
    //         return result.value( )->general_task_node.container->context.pcontext;
    //     }
    // }
    // else {
    //     while ( true );
    // }
    return frame;
}
auto SymmetricMultiprocessingSwitchCPUEntry::do_register( void ) -> void {
}
}     // namespace QuantumNEC::Kernel::x86_64