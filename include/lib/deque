#pragma once
#include <deque>
#include <kernel/memory/allocator/kheap.hpp>

namespace std {
template < typename T >
using cxxdeque = std::deque< T, Memory::KernelHeap::allocator< T > >;
}