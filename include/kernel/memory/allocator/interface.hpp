#pragma once
#include <cstdint>
namespace Memory {
template < typename T >     // type of objects
class allocator {
public:
    using value_type      = T;
    using pointer         = value_type *;
    using const_pointer   = const pointer;
    using reference       = value_type &;
    using const_reference = const value_type &;

public:
    allocator( void ) {
    }
    virtual ~allocator( void ) {
    }

public:     // allocate, collect interface
    /**
     * @brief allocate pages and construct objects in allocated storage
     * page_count -> the number of pages that you need
     */
    virtual auto allocate( std::size_t page_count ) -> pointer = 0;
    /**
     * @brief collect pages and destruct objects in allocated storage
     * address -> the pages that wait for destroying
     */
    virtual auto collect( const_pointer address ) -> void = 0;
};
}     // namespace Memory