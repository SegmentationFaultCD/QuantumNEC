#pragma once
#include <kernel/task/general/pcb/pcb.hpp>
#include <lib/Uefi.hpp>
namespace QuantumNEC::Kernel {
class Process {
public:
    enum class ErrorCode {
        ERROR_TASK_TYPE,
        ALLOCATE_MEMORY_FAULT
    };

public:
    explicit Process( const char_t *name, uint64_t priority, void *entry, PCB::Flags::Type type ) noexcept;
    explicit Process( Process &&process ) noexcept;

    Process( const Process &process ) noexcept                     = delete;
    auto operator=( const Process &process ) noexcept -> Process & = delete;

public:
    auto join( void ) noexcept -> void;

private:
    PCB *pcb;
    bool has_inserted;
};
}     // namespace QuantumNEC::Kernel