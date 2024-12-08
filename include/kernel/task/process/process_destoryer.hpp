#pragma once
#include <kernel/task/general/pcb/pcb.hpp>
#include <libcxx/expected.hpp>
namespace QuantumNEC::Kernel {
class ProcessDestoryer {
public:
    enum class ErrorCode {

    };

public:
    explicit ProcessDestoryer( void ) = default;

public:
    auto destory( PCB *pcb ) -> void;
};
}     // namespace QuantumNEC::Kernel