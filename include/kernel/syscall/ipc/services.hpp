#pragma once
#include <compare>
#include <kernel/memory/heap/kheap/kheap_allocater.hpp>
#include <kernel/task/general/scheduler/scheduler.hpp>
#include <lib/rbtree.hpp>
#include <numeric>
#include <tuple>
namespace QuantumNEC::Kernel {

class Service {
public:
    enum class ReceiveSource : uint64_t {
        ANY       = std::numeric_limits< uint64_t >::max( ),
        INTERRUPT = std::numeric_limits< uint64_t >::max( ) - 1,
    };

    struct Order {
        void    *shared_memory_space;
        uint64_t source;
        // 一些操作，后续扩展
    };

private:
    Lib::RedBlackTree< Order, uint64_t >       pool;
    Lib::RedBlackTree< Order, uint64_t >::Node block;

public:
    auto send( Order *order, Service *receiver ) {
        receiver->pool.insert( this->block );
        this->block.data( order );
        this->block.key( order->source );
    }

    auto receive( uint64_t source ) {
        if ( source == std::to_underlying( ReceiveSource::ANY ) || source == std::to_underlying( ReceiveSource::INTERRUPT ) ) {
            // TODO :
        }
        else {
            auto sender = pool.search( source );
            pool.remove( sender );

            return sender->shared_memory_space;
        }
    }
};
// 首先， 调用系统调用

}     // namespace QuantumNEC::Kernel