#include <kernel/global/x86_64/global.hpp>
#include <kernel/interrupt/interrupt.hpp>
#include <kernel/interrupt/x86_64/entry/entry_smp_switch_cpu.hpp>
#include <kernel/interrupt/x86_64/entry/idt.hpp>
#include <kernel/interrupt/x86_64/pic/pic.hpp>
#include <kernel/print.hpp>
#include <kernel/task/task.hpp>
#include <lib/spinlock.hpp>
using namespace QuantumNEC;
namespace QuantumNEC::Kernel::x86_64 {

auto SymmetricMultiprocessingSwitchCPUEntry::name( void ) noexcept -> void {
}
auto SymmetricMultiprocessingSwitchCPUEntry::error_code( [[maybe_unused]] uint64_t error_code ) noexcept -> void {
}
auto SymmetricMultiprocessingSwitchCPUEntry::handler( Frame *frame ) noexcept -> Frame * {
    Lib::spinlock lock;
    Apic::eoi( frame->vector );
    lock.acquire( );
    if ( PCB::get_running_task( )->flags.task_type == PCB::__flags__::__type__::KERNEL_PROCESS ) {
        PCB::get_running_task( )->context.pcontext = (PCB::__context__::__process__ *)frame;
    }
    if ( auto result = Scheduler { }.schedule( ); result.has_value( ) ) {
        auto context = result.value( )->general_task_node->context.pcontext;
        lock.release( );
        return context;
    }
    else {
        lock.release( );
        return frame;
    }
}
auto SymmetricMultiprocessingSwitchCPUEntry::do_register( void ) -> void {
}
}     // namespace QuantumNEC::Kernel::x86_64