#include <kernel/cpu/cpu.hpp>
#include <kernel/print.hpp>
#include <kernel/syscall/message.hpp>
namespace QuantumNEC::Kernel {
using namespace Lib;
using namespace Kernel;
/**
 * 第一个系统调用
 * 它的作用是进行进程间通信，消息发送/接收
 */
auto syscall_send_receive( IN Interrupt::InterruptFrame *frame ) -> Interrupt::InterruptFrame * {
    auto status { Syscall::Status::ERROR };
    auto function { static_cast< Syscall::Function >( frame->regs.rax ) };
    auto source_destination { frame->regs.rdi };
    switch ( auto message { reinterpret_cast< Message * >( frame->regs.rsi ) }; function ) {
    case Syscall::Function::SYSCALL_MESSAGE_SEND:
        status = message->send( source_destination );
        break;
    case Syscall::Function::SYSCALL_MESSAGE_RECEIVE:
        status = message->receive( source_destination );
        break;
    default:
        status = Syscall::Status::NO_SYSCALL;
        break;
    }
    // if ( status != SyscallStatus::SUCCESS ) {
    //     println< ostream::HeadLevel::ERROR >( "{} called {} syscall error! To PID:{}({})", Kernel::get_current< Kernel::Process >( )->name, static_cast< uint32_t >( function ), source_destination, static_cast< int32_t >( status ) );
    //     while ( TRUE )
    //         ;
    // }
    // if ( !( Task::ready_task->flags & Kernel::TASK_FLAG_THREAD ) ) {
    //     Task::ready_task->context->iframe.regs.rax = static_cast< uint64_t >( status );
    //     return &Task::ready_task->context->iframe;
    // }
    // else {
    return frame;
    // }
}

auto Message::send( IN uint64_t destination ) -> Syscall::Status {
    // auto sender { get_current< Process >( ) }, receiver { sender->find( destination ) };

    // using enum SyscallStatus;
    // sender->message.send_to = destination;
    // if ( !receiver ) {
    //     sender->message.send_to = NO_TASK;
    //     return NO_DESTINATION;
    // }
    // sender->message.source = sender->PID;
    // // 能构成一个环，三者一直发消息，这叫做死锁
    // if ( auto is_deadlock = [ & ] -> BOOL {
    //          if ( sender->PID == static_cast< decltype( sender->PID ) >( destination ) ) {
    //              return TRUE;
    //          }
    //          auto task { sender->find( destination ) };
    //          for ( ;; ) {
    //              if ( task->status == TaskStatus::SENDING ) {
    //                  if ( static_cast< decltype( sender->PID ) >( task->message.send_to ) == sender->PID ) {
    //                      return TRUE;     // 是死锁了
    //                  }
    //                  task = sender->find( task->message.send_to );
    //                  if ( !task ) {
    //                      return FALSE;
    //                  }
    //              }
    //              else {
    //                  break;
    //              }
    //          }
    //          return FALSE;
    //      };
    //      is_deadlock( ) ) {
    //     // 死锁就滚
    //     return DEADLOCK;
    // }
    // // 把这个当前消息拷贝给发送者
    // memcpy( &sender->message, this, sizeof *this );
    // // 把这个发送者添加到接收者的消息队列等候处理
    // // receiver->message.sender_list.append( sender->general_task_queue );

    // receiver->unblock( );
    // // 阻塞自己
    // sender->block( TaskStatus::SENDING );

    return Syscall::Status::SUCCESS;
}

auto Message::receive( IN uint64_t source ) -> Syscall::Status {
    // auto receiver { get_current< Process >( ) }, sender { (Process *)nullptr };
    // using enum SyscallStatus;
    // if ( source == receiver->PID ) {
    //     // 接收者也是发送者，死锁
    //     return DEADLOCK;
    // }
    // receiver->message.receive_from = source;
    // if ( source == ANY || source == INTERRUPT ) {
    //     // 从任意地方接收消息
    //     if ( receiver->message.has_int_message ) {
    //         // 消息来自中断
    //         this->source = INTERRUPT;
    //         this->type = INTERRUPT;
    //         receiver->message.has_int_message = 0;
    //     }
    //     receiver->block( TaskStatus::RECEIVING );
    //     return SUCCESS;
    //     // if ( receiver->message.sender_list.is_empty( ) ) {
    //     //     // 接收列表是空的话就把自己阻塞了
    //     //     receiver->block( TaskStatus::RECEIVING );
    //     //     return SUCCESS;
    //     // }
    //     // else {
    //     //     // 不是空的说明有消息待处理
    //     //     sender = reinterpret_cast< Process * >( receiver->message.sender_list.pop( )->container );
    //     //     // 那么就弹出第一个，因为级别最高
    //     //     memcpy( this, &sender->message, sizeof *this );
    //     //     // 把消息复制给自己
    //     //     sender->message.send_to = NO_TASK;
    //     //     sender->message.receive_from = receiver->PID;
    //     //     receiver->message.receive_from = NO_TASK;
    //     //     sender->block( TaskStatus::RECEIVING );     // 把对方设置为接收状态
    //     //     return SUCCESS;
    //     // }
    // }
    // else {
    //     // 这种一般都是回复接收者
    //     // 对于这种情况sender就是原来的接收者，receiver是原来的发送者
    //     // 将接收者从发送者的消息发送者队列中移除
    //     sender = receiver->find( source );
    //     if ( !sender ) {
    //         // 根本没有这个接收者
    //         receiver->message.receive_from = NO_TASK;
    //         return SyscallStatus::NO_SOURCE;
    //     }

    //     // if ( !receiver->message.sender_list.find( sender->general_task_queue ) ) {
    //     //     // 如果对方没给自己发送过消息，那么阻塞
    //     //     receiver->block( TaskStatus::RECEIVING );
    //     //     sender->unblock( );
    //     //     return Architecture::ArchitectureManager< TARGET_ARCH >::Syscall::SyscallStatus::SUCCESS;
    //     // }
    //     // else {
    //     //     // 发送了，没处理，那么现在开始处理
    //     //     sender->message.sender_list.remove( receiver->general_task_queue );
    //     //     receiver->message.sender_list.remove( sender->general_task_queue );
    //     // 把对面传递回来的消息复制
    //     memcpy( this, &sender->message, sizeof *this );
    //     sender->message.send_to = NO_TASK;
    //     // if ( sender->status == TaskStatus::SENDING ) {
    //     //     // 对面处于发送状态那么就让对面变得正常
    //     //     sender->unblock( );
    //     // }
    //     receiver->unblock( );
    //     sender->unblock( );

    //     receiver->message.receive_from = NO_TASK;
    //     return SyscallStatus::SUCCESS;
    //     //}
    // }
}
}     // namespace QuantumNEC::Kernel
