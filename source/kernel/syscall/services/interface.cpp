#include <kernel/syscall/services/interface.hpp>
#include <kernel/task/schedule/scheduler.hpp>
#include <kernel/task/task.hpp>
namespace Kernel {
auto Servicer::enable( ) -> void {
    Task::scheduler->first_initialize( *this->service );
    // this->service->schedule->priority = 0;
    Task::scheduler->wake_up( std::move( *this->service ) );
}
auto Servicer::disable( ) -> void {}
}     // namespace Kernel