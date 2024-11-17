#pragma once
#include <kernel/task/general/pcb/pcb.hpp>
#include <libcxx/expected.hpp>
PUBLIC namespace QuantumNEC::Kernel {
    class ProcessCreater {
    public:
        enum class ErrorCode {
            ERROR_TASK_TYPE,
            ALLOCATE_MEMORY_FAULT
        };

    public:
        explicit ProcessCreater( VOID )  = default;
        ~ProcessCreater( VOID ) noexcept = default;

    public:
        auto create( CONST char_t *_name_, uint64_t _priority_, VOID *_entry_, PCB::Type type ) -> std::expected< PCB *, ErrorCode >;
    };
}