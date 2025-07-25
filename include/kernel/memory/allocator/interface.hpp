#pragma once
#include <cstdint>
namespace Memory {
template < typename T >     // type of objects
class allocator {
public:
    using value_type    = T;
    using pointer       = value_type *;
    using const_pointer = const pointer;

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
    virtual auto allocate( std::size_t page_count ) -> pointer = 0;
    /**
     * @brief collect memory and destruct objects in allocated storage
     * address -> the memory that wait for destroying
     */
    virtual auto deallocate( const_pointer address ) -> void = 0;
};

}     // namespace Memory