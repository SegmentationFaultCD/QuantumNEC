#include <kernel/syscall/ipc/send.hpp>
#include <kernel/syscall/syscall.hpp>
namespace QuantumNEC::Kernel {
auto MessageSender::execute_order( uint64_t servicer_index, const message &passed_messages ) -> result {
    auto receiver = Syscall::get_servicer( Syscall::Servicer { servicer_index } ).raw_control_block( );

    this->role.sender.set_receiver( receiver, passed_messages );

    this->source = ProcessControlBlock::get_running_task( );

    if ( this->is_deadlock( this->source->PID, receiver->PID ) ) {
        std::println< std::print_level::ERROR >( "the event that a task [{}] send messages to another task [{}] is deadlock.", this->source->name, receiver->name );
        return result::DEADLOCK;
    }
    if ( !receiver->ipc ) {
        std::println< std::print_level::ERROR >( "Servicer [{}] isn't available!", receiver->name );
        return result::SERVICER_NOT_AVAILABLE;
    }

    if ( receiver->schedule.state == decltype( receiver->schedule.state )::RECEIVING ) {
        if ( auto receive_from = receiver->ipc->role.receiver.get_receive_from( ).first;
             receive_from == RECEIVE_FROM_ANY || receive_from == this->source->PID ) {
            // 解锁
        }
    }
    // 给自己上锁
    return result::SUCCESS;
}
}     // namespace QuantumNEC::Kernel
