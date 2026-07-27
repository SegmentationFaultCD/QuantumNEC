#include <kernel/task/schedule/scheduler.hpp>
namespace Task {
template <>
auto Scheduler< Muqss >::initialize( void ) -> void {
    scheduler = new Scheduler;
}

template <>
auto Scheduler< Muqss >::get_current( ) -> Core & {
    return scheduler->running_queue[ Interrupt::apic.apic_id( ) ];
}

}     // namespace Task
