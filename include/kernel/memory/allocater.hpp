#pragma once
#include <lib/Uefi.hpp>
#include <type_traits>
namespace QuantumNEC::Kernel {

template < typename T, typename SizeType = size_t >
struct allocation_result {
    T       *ptr;
    SizeType count;
};

template < class Alloc >
class allocator_traits {
public:
    using allocator_type  = Alloc;
    using type            = Alloc::type;
    using difference_type = Alloc::difference_type;
    using size_type       = Alloc::size_type;

public:
    static auto allocate( Alloc &a, size_type n ) {
        return a.allocate( n );
    }
    static auto allocate_at_least( Alloc &a, size_type n ) {
        return a.allocate_at_least( n );
    }
    static auto deallocate( Alloc &a, type *p, size_type n ) -> void {
        a.deallocate( p, n );
    }
    static auto construct( [[maybe_unused]] Alloc &a, type *p, auto &&...args ) {
        if constexpr ( std::is_array_v< type > ) {
            new ( reinterpret_cast< void * >( p ) ) type[ 1 ] { };
        }
        else {
            new ( reinterpret_cast< void * >( p ) ) type { args... };
        }
    }
};
}     // namespace QuantumNEC::Kernel