#include <Kernel/Task/message.hpp>
#include <Kernel/Task/process.hpp>
#include <Kernel/Task/thread.hpp>
#include <Lib/IO/Stream/iostream>
#include <cstring>
PUBLIC namespace QuantumNEC::Kernel {
    auto Message::send_receive( IN Architecture::ArchitectureManager< TARGET_ARCH >::SyscallFunction function, IN Lib::Types::uint64_t source_destination )->Architecture::ArchitectureManager< TARGET_ARCH >::SyscallStatus {
        using namespace Architecture;
        ArchitectureManager< TARGET_ARCH >::SyscallStatus status { };
        using enum ArchitectureManager< TARGET_ARCH >::SyscallFunction;
        using enum ArchitectureManager< TARGET_ARCH >::SyscallStatus;
        switch ( function ) {
        case MESSAGE_SEND:
        case MESSAGE_RECEIVE:
            // RAX放系统调用的功能号，RDI放源进程或者要被服务进程，RSI就是message，并用RAX接收返回值
            status = Architecture::__syscall( function, source_destination, this, 0, 0, 0, 0 );
            break;
        case MESSAGE_SEND_RECEIVE:
            status = Architecture::__syscall( MESSAGE_SEND, source_destination, this, 0, 0, 0, 0 );
            if ( status == SUCCESS ) {
                status = Architecture::__syscall( MESSAGE_RECEIVE, source_destination, this, 0, 0, 0, 0 );
            }
            break;
        default:
            break;
        }
        return status;
    }
    auto Message::send( IN Lib::Types::uint64_t destination )->Architecture::ArchitectureManager< TARGET_ARCH >::Syscall::SyscallStatus {
        auto sender { get_current< Process >( ) }, receiver { sender->find( destination ) };

        sender->message.send_to = destination;
        if ( !receiver ) {
            this->send_to = NO_TASK;
            return Architecture::ArchitectureManager< TARGET_ARCH >::Syscall::SyscallStatus::NO_DESTINATION;
        }
        this->source = sender->PID;
        // 能构成一个环，三者一直发消息，这叫做死锁
        if ( auto is_deadlock = [ & ] -> Lib::Types::BOOL {
                 if ( sender->PID == static_cast< decltype( sender->PID ) >( destination ) ) {
                     return TRUE;
                 }
                 auto task { sender->find( destination ) };
                 for ( ;; ) {
                     if ( task->status == TaskStatus::SENDING ) {
                         if ( static_cast< decltype( sender->PID ) >( task->message.send_to ) == sender->PID ) {
                             return TRUE;     // 是死锁了
                         }
                         task = sender->find( task->message.send_to );
                         if ( !task ) {
                             return FALSE;
                         }
                     }
                     else {
                         break;
                     }
                 }
                 return FALSE;
             };
             is_deadlock( ) ) {
            // 死锁就滚
            return Architecture::ArchitectureManager< TARGET_ARCH >::Syscall::SyscallStatus::DEADLOCK;
        }

        // 把这个当前消息拷贝给发送者
        Lib::STL::memcpy( &sender->message, this, sizeof *this );
        // 把这个发送者PCB地址添加到接收者的消息队列
        Lib::STL::list_add_to_end( &receiver->message.sender_list, &sender->general_task_queue );
        // if ( ( receiver->status == TaskStatus::RECEIVING ) && ( receiver->message.receive_from == ANY || static_cast< Lib::Types::uint64_t >( receiver->message.receive_from ) == sender->PID ) ) {
        //     // 对方正准备接收消息 ，并且接收消息来自任何地方或接收发送者的消息
        //     // 解除对面阻塞
        //     sender->unblock( receiver );
        // }
        // 阻塞自己

        sender->unblock( receiver );
        sender->block( TaskStatus::SENDING );
        return Architecture::ArchitectureManager< TARGET_ARCH >::Syscall::SyscallStatus::SUCCESS;
    }

    auto Message::receive( IN Lib::Types::uint64_t source )->Architecture::ArchitectureManager< TARGET_ARCH >::Syscall::SyscallStatus {
        auto receiver { get_current< Process >( ) }, sender { (Process *)NULL };

        if ( source == receiver->PID ) {
            // 接收者也是发送者，死锁
            return Architecture::ArchitectureManager< TARGET_ARCH >::Syscall::SyscallStatus::DEADLOCK;
        }
        receiver->message.receive_from = source;
        if ( source == ANY || source == INTERRUPT ) {
            // 从任意地方接收消息
            if ( receiver->message.has_int_message ) {
                // 消息来自中断
                this->source = INTERRUPT;
                this->type = INTERRUPT;
                receiver->message.has_int_message = 0;
            }
            if ( Lib::STL::list_is_empty( &receiver->message.sender_list ) ) {
                // 接收列表是空的话就把自己阻塞了
                receiver->block( TaskStatus::RECEIVING );
                return Architecture::ArchitectureManager< TARGET_ARCH >::Syscall::SyscallStatus::SUCCESS;
            }
        }
        else {
            auto sender = receiver->find( source );
            if ( !sender ) {
                // 根本没有这个发送者
                receiver->message.receive_from = NO_TASK;
                return Architecture::ArchitectureManager< TARGET_ARCH >::Syscall::SyscallStatus::NO_SOURCE;
            }
            if ( !Lib::STL::list_find( &receiver->message.sender_list, &sender->general_task_queue ) ) {
                // 如果对方没给自己发送过消息，那么阻塞
                receiver->block( TaskStatus::RECEIVING );
                return Architecture::ArchitectureManager< TARGET_ARCH >::Syscall::SyscallStatus::SUCCESS;
            }
        }
        // 发送了，没处理，那么现在开始处理
        // sender = reinterpret_cast< Process * >( Lib::STL::list_pop( &receiver->message.sender_list )->container );
        // if ( !sender ) {
        //     receiver->message.receive_from = NO_TASK;
        //     return Architecture::ArchitectureManager< TARGET_ARCH >::Syscall::SyscallStatus::NO_SOURCE;
        // }
        // 把发送者的消息复制
        Lib::STL::memcpy( this, &sender->message, sizeof *this );
        sender->message.send_to = NO_TASK;
        if ( sender->status == TaskStatus::SENDING ) {
            receiver->unblock( sender );
        }
        // receiver->ticks += sender->ticks;
        // sender->ticks = 0;
        receiver->message.receive_from = NO_TASK;
        return Architecture::ArchitectureManager< TARGET_ARCH >::Syscall::SyscallStatus::SUCCESS;
    }
}
