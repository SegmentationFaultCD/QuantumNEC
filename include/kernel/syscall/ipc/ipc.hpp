#pragma once
#include <kernel/memory/heap/kheap/kheap_allocater.hpp>
#include <kernel/task/general/scheduler/scheduler.hpp>
#include <lib/rbtree.hpp>
#include <tuple>
namespace QuantumNEC::Kernel {
template < typename TaskControlBlock >
class InterprocessCommunication {
public:
    enum class result : uint64_t {
        DEADLOCK,
        SUCCESS,
        SERVICER_NOT_AVAILABLE,
        HAVE_NO_SENDER,
    };
    constexpr static auto RECEIVE_FROM_ANY { ~0ull };
    constexpr static auto RECEIVE_FROM_INTERRUPT { ~0ull - 1 };

    class message {
    public:
        message( uint64_t a1 = 0, uint64_t a2 = 0, uint64_t a3 = 0, uint64_t a4 = 0, uint64_t a5 = 0, uint64_t a6 = 0 ) {
            this->messages[ 0 ] = a1;
            this->messages[ 1 ] = a2;
            this->messages[ 2 ] = a3;
            this->messages[ 3 ] = a4;
            this->messages[ 4 ] = a5;
            this->messages[ 5 ] = a6;
            this->source        = TaskControlBlock::get_running_task( )->PID;
        }

        auto change_message( uint64_t index, uint64_t message ) const {
            this->messages[ index ] = message;
        }

        // for rbtree.
        auto operator<( const message &other ) -> bool {
            return this->source < other.source;
        }
        auto operator>( const message &other ) -> bool {
            return this->source > other.source;
        }
        auto operator=( const message &other ) -> message & {
            this->source = other.source;
            std::memcpy( this->messages, const_cast< uint64_t * >( other.messages ), sizeof( uint64_t ) * 6 );
        }
        auto get_PID( void ) {
            return source;
        }

    private:
        uint64_t         source;
        mutable uint64_t messages[ 6 ];
    };

public:
    explicit InterprocessCommunication( void ) noexcept {
    }
    virtual ~InterprocessCommunication( void ) noexcept {
    }

    virtual auto execute_order( uint64_t servicer_index, const message &messages ) -> result = 0;

public:
    struct s {
        // 作为user时使用
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
            auto set_receiver( const TaskControlBlock *tcb, const message &messages ) {
                this->node    = { const_cast< message * >( &messages ), tcb->PID };
                this->send_to = { tcb->PID, tcb->schedule.priority };
            }
        } sender;

        class __receiver__ {
            friend InterprocessCommunication;

        public:
            __receiver__( ) {
            }

        private:
            // 作为servicer时使用
            Lib::RedBlackTree< message, uint64_t >
                                            sender_tree;
            std::pair< uint64_t, uint64_t > receive_from;

        public:
            auto join_wait( Lib::RedBlackTree< message, uint64_t >::Node sender ) {
                this->sender_tree.insert( sender );
            }
            auto &get_receive_from( ) {
                return receive_from;
            }
            auto sender_tree_empty( ) {
                return this->sender_tree.empty( );
            }
            auto &get_a_sender( ) {
                return sender_tree.left( );
            }
            auto find_of( uint64_t PID ) -> std::expected< typename Lib::RedBlackTree< message, uint64_t >::Node *, result > {
                if ( auto sender = this->sender_tree.search( PID ); sender ) {
                    return sender;
                }
                return std::unexpected { result::HAVE_NO_SENDER };
            }
            auto remove_sender( uint64_t PID ) {
                this->sender_tree.remove( Scheduler { }.search( PID ) );
            }
        } receiver;
    } role;
    template < bool operation >
    auto has_intrrupt_message( ) {
        if constexpr ( operation ) {
            return this->_has_intrrupt_message;
        }
        else {
            this->_has_intrrupt_message = false;
        }
    }

protected:
    TaskControlBlock *source;

    bool _has_intrrupt_message;

    uint64_t type;

    auto is_deadlock( uint64_t source, uint64_t destination ) -> bool {
        if ( source == destination ) {
            return true;
        }
        if ( auto dest = TaskControlBlock::get_running_task( ); dest->schedule.state == decltype( dest->schedule.state )::SENDING ) {
            if ( dest->ipc ) {
                if ( dest->ipc->role.sender.send_to.first == source ) {
                    return true;
                }
                dest = Scheduler { }.search( dest->ipc->role.sender.send_to.second, dest->ipc->role.sender.send_to.first );
                if ( !dest ) {
                    // 空的不在死锁定义范围之内
                    return false;
                }
            }
            else {
                // 挂了
            }
        }
        return false;
    }

    message messages;
};

}     // namespace QuantumNEC::Kernel