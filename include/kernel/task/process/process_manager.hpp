#pragma once
#include <kernel/task/general/pcb/pcb.hpp>
#include <lib/Uefi.hpp>
PUBLIC namespace QuantumNEC::Kernel {
    class ProcessManager {
    public:
        explicit ProcessManager( VOID ) noexcept;
        ~ProcessManager( VOID ) noexcept = default;
    };
}