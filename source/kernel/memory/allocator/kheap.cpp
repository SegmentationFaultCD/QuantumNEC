#include <cstdint>
#include <kernel/driver/serial_port/serial_port.hpp>
#include <kernel/memory/allocator/kheap.hpp>
#include <memory>
namespace Memory::KernelHeap {
auto initialize( void ) -> void {
    using namespace Page;
    Page::allocator< Type::P2Mib > page_allocator;
    auto zone = (uint64_t)physical_to_virtual( page_allocator.allocate( allocator< void >::cache_size_count ) );

    for ( auto i = 0ul; i < allocator< void >::cache_size_count; ++i ) {
        auto &slab_cache = allocator_manager::slab_caches[ i ];
        std::construct_at( &slab_cache.pool_list );
        slab_cache.size = allocator< void >::cache_size[ i ];
        std::construct_at( reinterpret_cast< Slab * >( zone ) );
        slab_cache.cache_pool = reinterpret_cast< Slab * >( zone );
        zone += sizeof( Slab ) + sizeof( uint64_t ) * 10;
        slab_cache.cache_pool->using_count = 0;
        slab_cache.cache_pool->free_count = page_allocator.__page_size__ / allocator< void >::cache_size[ i ];
        slab_cache.cache_pool->color_length = ( ( page_allocator.__page_size__ / allocator< void >::cache_size[ i ] + sizeof( uint64_t ) * 8 - 1 ) >> 6 ) << 3;
        slab_cache.cache_pool->color_count = slab_cache.cache_pool->free_count;
        slab_cache.cache_pool->color_map = (uint64_t *)zone;
        std::memset( slab_cache.cache_pool->color_map, 0xff, slab_cache.cache_pool->color_length );
        zone = ( zone + slab_cache.cache_pool->color_length + sizeof( uint64_t ) * 10 ) & ( ~sizeof( uint64_t ) - 1 );
        for ( auto j = 0ul; j < slab_cache.cache_pool->color_count; ++j ) {
            *( slab_cache.cache_pool->color_map + ( j >> 6 ) ) ^= 1ul << j % 64;
        }
        slab_cache.total_free = slab_cache.cache_pool->color_count;
        slab_cache.total_using = 0;

        slab_cache.cache_pool->page = std::allocator_traits< decltype( page_allocator ) >::allocate( page_allocator, 1 );
        slab_cache.cache_pool->virtual_address = reinterpret_cast< void * >( physical_to_virtual( slab_cache.cache_pool->page ) );
        slab_cache.cache_pool->list = *slab_cache.cache_pool;
        slab_cache.pool_list.append( slab_cache.cache_pool->list );
    }
}

}     // namespace Memory::KernelHeap
auto operator new( std::size_t size ) -> void * {
    Memory::KernelHeap::allocator< void > a;
    return a.allocate( size );
}
auto operator new[]( std::size_t size ) -> void * {
    return operator new( size );
}
auto operator new[]( unsigned long size, std::align_val_t ) -> void * {
    return operator new( size );
}
auto operator delete( void *address ) noexcept -> void {
    Memory::KernelHeap::allocator< void > c;
    c.deallocate( address );
    return;
}
auto operator delete[]( void *address ) noexcept -> void {
    operator delete( address );
    return;
}
auto operator delete( void *, unsigned long ) noexcept -> void {
    return;
}
auto operator delete[]( void *, unsigned long ) noexcept -> void {
    return;
}
