#pragma once
#include <type_traits>
namespace Memory {
template < typename T >     // type of objects
class allocator {
public:
    allocator( void ) {
    }
    virtual ~allocator( void ) {
    }

public:     // allocate, collect interface
    /**
     * @brief allocate memory and construct objects in allocated storage
     * page_count -> the size of memory that you need
     */
    virtual auto allocate( std::size_t page_count ) -> T * = 0;
    /**
     * @brief collect memory and destruct objects in allocated storage
     * address -> the memory that wait for destroying
     */
    virtual auto deallocate( const T *address, std::size_t size ) -> void = 0;
};

template < typename T, typename SizeType = std::size_t >
struct allocation_result {
    T *ptr;
    SizeType count;
};

template < class Alloc >
    requires requires {
        typename Alloc::type;
        typename Alloc::difference_type;
        typename Alloc::size_type;
    }
class allocator_traits {
public:
    using allocator_type = Alloc;
    using type = Alloc::type;
    using difference_type = Alloc::difference_type;
    using size_type = Alloc::size_type;

public:
    static auto allocate( Alloc &a, size_type n ) {
        return a.allocate( n );
    }
    static auto deallocate( Alloc &a, type *p, size_type n ) -> void {
        a.deallocate( p, n );
    }
    static auto construct( [[maybe_unused]] Alloc &a, type *p, auto &&...args ) {
        if constexpr ( std::is_array_v< type > ) {
            new ( reinterpret_cast< void * >( p ) ) type[ 1 ] {};
        }
        else {
            new ( reinterpret_cast< void * >( p ) ) type { args... };
        }
    }
};

}     // namespace Memory