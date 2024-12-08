#pragma once
#include <kernel/task/general/pcb/pcb.hpp>
#include <libcxx/expected.hpp>
namespace QuantumNEC::Kernel {
class ProcessCreater {
public:
    enum class ErrorCode {
        ERROR_TASK_TYPE,
        ALLOCATE_MEMORY_FAULT
    };

public:
    explicit ProcessCreater( void )  = default;
    ~ProcessCreater( void ) noexcept = default;

public:
    auto create( const char_t *_name_, uint64_t _priority_, void *_entry_, PCB::Type type ) -> std::expected< PCB *, ErrorCode >;
};
}     // namespace QuantumNEC::Kernel