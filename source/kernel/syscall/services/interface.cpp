#include <kernel/syscall/services/interface.hpp>
#include <kernel/task/schedule/MuQss.hpp>
#include <kernel/task/schedule/scheduler.hpp>
#include <kernel/task/task.hpp>
namespace Kernel {
auto Servicer::enable( ) -> void {
    this->service->schedule->hw_scheduler->insert( this->service );
 
}
auto Servicer::disable( ) -> void {}
}     // namespace Kernel