#pragma once
#include <kernel/syscall/ipc/ipc.hpp>
#include <kernel/task/general/pcb/pcb.hpp>
#include <lib/Uefi.hpp>
namespace QuantumNEC::Kernel {
class MessageReceiver :
    public InterprocessCommunication< ProcessControlBlock > {
public:
    explicit MessageReceiver( void ) noexcept {
    }

    virtual ~MessageReceiver( void ) noexcept {
    }

public:
    virtual auto execute_order( uint64_t source, const message &messages ) -> result override;
};
}     // namespace QuantumNEC::Kernel