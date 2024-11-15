#pragma once
#include <kernel/task/manager/pcb.hpp>
#include <libcxx/expected.hpp>
PUBLIC namespace QuantumNEC::Kernel {
    class ProcessCreater {
    public:
        enum class ErrorCode {

        };

    public:
        explicit ProcessCreater( VOID ) = default;

    public:
        auto create( CONST char_t *_name_, uint64_t _priority_, VOID *_entry_ ) -> std::expected< PCB *, ErrorCode >;
    };
}