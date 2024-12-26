#pragma once
#include <lib/Uefi.hpp>
namespace QuantumNEC::Kernel {
class PCB;
class ProcessManager {
public:
    explicit ProcessManager( void ) noexcept;
    ~ProcessManager( void ) noexcept = default;

public:
    static auto main_process_install( uint64_t core ) -> PCB *;

    static auto get_running_task( void ) -> PCB *;

    static inline PCB *main_pcb;
};
}     // namespace QuantumNEC::Kernel