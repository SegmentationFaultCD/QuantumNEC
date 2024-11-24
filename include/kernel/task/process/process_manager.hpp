#pragma once
#include <kernel/task/general/pcb/pcb.hpp>
#include <lib/Uefi.hpp>
PUBLIC namespace QuantumNEC::Kernel {
    class ProcessManager {
    public:
        explicit ProcessManager( VOID ) noexcept;
        ~ProcessManager( VOID ) noexcept = default;

    public:
        STATIC auto get_running_task( VOID ) -> PCB *;

        static inline PCB *main_pcb;
    };
}