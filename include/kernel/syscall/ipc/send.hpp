#pragma once
#include <kernel/syscall/ipc/ipc.hpp>
#include <kernel/task/general/pcb/pcb.hpp>
#include <lib/Uefi.hpp>
namespace QuantumNEC::Kernel {
class MessageSender :
    public InterprocessCommunication< ProcessControlBlock > {
public:
    class __sender__ {
        friend InterprocessCommunication;

    public:
        __sender__( ) {
        }

    private:
        Lib::RedBlackTree< message, uint64_t /* PID */ >::Node node;
        std::pair< uint64_t,      // PID
                   uint64_t >     // priority
            send_to;

    public:
        auto set_receiver( const ProcessControlBlock *tcb, const message &messages ) {
            this->node    = { const_cast< message * >( &messages ), tcb->PID };
            this->send_to = { tcb->PID, tcb->schedule.priority };
        }
    } sender;

    explicit MessageSender( void ) noexcept {
    }

    virtual ~MessageSender( void ) noexcept {
    }

public:
    virtual auto execute_order( uint64_t servicer_index, const message &messages ) -> result override;
};
}     // namespace QuantumNEC::Kernel