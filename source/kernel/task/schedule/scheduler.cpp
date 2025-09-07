#include <kernel/task/schedule/MuQss.hpp>
#include <kernel/task/schedule/scheduler.hpp>
namespace Task {
auto Scheduler::initialize( void ) -> void {
    scheduler = new MuQss;
}
}     // namespace Task
