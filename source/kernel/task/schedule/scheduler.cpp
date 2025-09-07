#include <kernel/task/schedule/MuQss.hpp>
#include <kernel/task/schedule/scheduler.hpp>
namespace Task {
auto initialize_scheduler( void ) -> void {
    scheduler = new MuQss;
}
}     // namespace Task
