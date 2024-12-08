#pragma once
#include <kernel/task/general/pcb/pcb.hpp>
#include <lib/Uefi.hpp>
namespace QuantumNEC::Kernel {
class ProcessManager {
public:
    explicit ProcessManager( void ) noexcept;
    ~ProcessManager( void ) noexcept = default;

public:
    static auto get_running_task( void ) -> PCB *;

    static inline PCB *main_pcb;
};
}     // namespace QuantumNEC::Kernel