#pragma once
#include <lib/Uefi.hpp>
namespace QuantumNEC::Kernel {
class ProcessControlBlock;
class ProcessManager {
public:
    explicit ProcessManager( void ) noexcept;
    ~ProcessManager( void ) noexcept = default;

public:
    static auto main_process_install( uint64_t core ) -> ProcessControlBlock *;

    static inline ProcessControlBlock *main_pcb;
};
}     // namespace QuantumNEC::Kernel