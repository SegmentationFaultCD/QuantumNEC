#pragma once
#include <compare>
#include <kernel/memory/heap/kheap/kheap_allocater.hpp>
#include <kernel/task/general/scheduler/scheduler.hpp>
#include <lib/list.hpp>
#include <numeric>
#include <tuple>
namespace QuantumNEC::Kernel {
template < typename TaskControlBlock >
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
    Lib::ListTable< TaskControlBlock >           waiter_queue;
    Lib::ListTable< TaskControlBlock >::ListNode node;
};
// 首先， 调用系统调用

}     // namespace QuantumNEC::Kernel