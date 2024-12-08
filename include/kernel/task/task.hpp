#pragma once
#include <lib/Uefi.hpp>

#define BFS
#ifdef BFS
#include <kernel/task/general/scheduler/bfs.hpp>
#elifdef CFS
#include <kernel/task/general/scheduler/cfs.hpp>
#elifdef EEVDFS
#include <kernel/task/general/scheduler/eevdfs.hpp>
#endif
#include <kernel/task/process/process_manager.hpp>
namespace QuantumNEC::Kernel {
using Scheduler = BrainFuckScheduler;
class Task :
    public SchedulerHelper,
    public ProcessManager {
public:
    explicit Task( void ) noexcept = default;
    ~Task( void ) noexcept         = default;
};
}     // namespace QuantumNEC::Kernel
