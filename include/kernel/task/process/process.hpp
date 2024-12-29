#pragma once
#include <kernel/task/general/pcb/pcb.hpp>
#include <lib/Uefi.hpp>
#include <modules/loader/loader.hpp>
namespace QuantumNEC::Kernel {
class Process {
public:
    constexpr static auto __user_process_text_segment_start__ { 0x400000ul };

public:
    enum class ErrorCode {
        ERROR_TASK_TYPE,
        ALLOCATE_MEMORY_FAULT
    };

public:
    explicit Process( const char_t *name, uint64_t priority, void *entry, PCB::Flags::Type type ) noexcept;
    explicit Process( Process &&process ) noexcept;
    explicit Process( const Modules::ModuleLoader::FileInformation &file, uint64_t priority, PCB::Flags::Type type ) noexcept;

    Process( const Process &process ) noexcept                     = delete;
    auto operator=( const Process &process ) noexcept -> Process & = delete;

public:
    auto join( void ) noexcept -> void;

private:
    PCB *pcb;
    bool has_inserted;
};
}     // namespace QuantumNEC::Kernel