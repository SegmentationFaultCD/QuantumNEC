#pragma once
#include <kernel/syscall/ipc/ipc.hpp>
#include <kernel/task/general/pcb/pcb.hpp>
#include <lib/Uefi.hpp>
namespace QuantumNEC::Kernel {
class MessageSender :
    public InterprocessCommunication< ProcessControlBlock > {
public:
    explicit MessageSender( void ) noexcept {
    }

    virtual ~MessageSender( void ) noexcept {
    }

public:
    virtual auto execute_order( uint64_t servicer_index, const message &messages ) -> result override;
};
}     // namespace QuantumNEC::Kernel