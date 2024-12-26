#pragma once
#include <lib/Uefi.hpp>

#include <kernel/task/general/pcb/pcb.hpp>
#include <kernel/task/process/process_manager.hpp>
namespace QuantumNEC::Kernel {

class Task :
    public SchedulerHelper,
    public ProcessManager {
public:
    explicit Task( void ) noexcept = default;
    ~Task( void ) noexcept         = default;
};
}     // namespace QuantumNEC::Kernel
