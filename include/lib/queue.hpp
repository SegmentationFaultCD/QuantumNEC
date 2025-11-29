#pragma once
#include "./deque.hpp"
#include <kernel/memory/allocator/kheap.hpp>
#include <queue>
namespace std {
template < typename T >
using cxxqueue = std::queue< T, cxxdeque< T > >;
}