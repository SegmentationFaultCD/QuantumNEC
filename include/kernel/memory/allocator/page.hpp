#pragma once
#include <kernel/memory/allocator/interface.hpp>
namespace Memory::Page {

auto loader_entry( void ) -> void;

template < typename T >
class allocator final : public Memory::allocator< T > {
    friend auto loader_entry( void ) -> void;

public:
    explicit allocator( void ) noexcept :
        Memory::allocator< T > { } {}
    virtual ~allocator( void ) {}

private:
    static auto initialize( ) -> void {
    }

public:
    virtual auto allocate( std::size_t page_count ) -> Memory::allocator< T >::pointer override {
    }
    virtual auto collect( std::size_t page_count ) -> void override {
    }

private:
};
}     // namespace Memory::Page