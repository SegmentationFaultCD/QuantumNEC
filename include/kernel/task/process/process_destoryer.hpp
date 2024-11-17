#pragma once
#include <kernel/task/general/pcb/pcb.hpp>
#include <libcxx/expected.hpp>
PUBLIC namespace QuantumNEC::Kernel {
    class ProcessDestoryer {
    public:
        enum class ErrorCode {

        };

    public:
        explicit ProcessDestoryer( VOID ) = default;

    public:
        auto destory( PCB *pcb ) -> VOID;
    };
}