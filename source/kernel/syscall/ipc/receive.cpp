#include <kernel/syscall/ipc/receive.hpp>
#include <kernel/syscall/syscall.hpp>
namespace QuantumNEC::Kernel {
auto MessageReceiver::execute_order( uint64_t source, const message &passed_messages ) -> result {
    this->source = ProcessControlBlock::get_running_task( );
    if ( source == this->source->PID ) {
        return result::DEADLOCK;
    }
    this->role.receiver.get_receive_from( ).first = source;

    if ( source == RECEIVE_FROM_ANY || source == RECEIVE_FROM_INTERRUPT ) {
        if ( this->role.receiver.sender_tree_empty( ) && !this->has_intrrupt_message< true >( ) ) {
            // 阻塞
        }
        if ( this->source->ipc ) {
            if ( this->source->ipc->has_intrrupt_message< true >( ) ) {
                this->source = NULL;
                this->type   = RECEIVE_FROM_INTERRUPT;
                passed_messages.change_message( 0, this->source->ipc->has_intrrupt_message< true >( ) );
                this->source->ipc->has_intrrupt_message< false >( );
                return result::SUCCESS;
            }
        }
        else {
            //
        }
        source = this->source->ipc->role.receiver.get_a_sender( ).get_PID( );
    }
    else {
        if ( auto result = this->role.receiver.find_of( source ); !result.has_value( ) ) {
            // 阻塞
        }
    }
    return result::SUCCESS;
}
}     // namespace QuantumNEC::Kernel
