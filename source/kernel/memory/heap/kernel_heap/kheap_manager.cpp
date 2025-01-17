#include <kernel/memory/heap/kheap/kheap_manager.hpp>
#include <kernel/memory/heap/slab/slab.hpp>
#include <kernel/memory/memory.hpp>
#include <kernel/memory/page/page_allocater.hpp>
#include <kernel/print.hpp>
#include <libcxx/string.hpp>
namespace QuantumNEC::Kernel {
inline PageAllocator< MemoryPageType::PAGE_2M > page_allocator { };

KHeapManager::KHeapManager( ) noexcept {
    auto zone = (uint64_t)physical_to_virtual( allocator_traits< decltype( page_allocator ) >::allocate( page_allocator, this->cache_size_count ) );

    for ( auto i = 0ul; i < this->cache_size_count; ++i ) {
        this->slab_caches[ i ].visit(
            [ &, this ]( const Lib::shared_spinlock< SlabCache > &slab_cache ) {
                slab_cache.value( ).pool_list.init( );
                slab_cache.value( ).size       = this->cache_size[ i ];
                slab_cache.value( ).cache_pool = new ( (void *)zone ) Slab { };
                zone += sizeof( Slab ) + sizeof( uint64_t ) * 10;
                slab_cache.value( ).cache_pool->using_count  = 0;
                slab_cache.value( ).cache_pool->free_count   = PageAllocator< MemoryPageType::PAGE_2M >::__page_size__ / this->cache_size[ i ];
                slab_cache.value( ).cache_pool->color_length = ( ( PageAllocator< MemoryPageType::PAGE_2M >::__page_size__ / this->cache_size[ i ] + sizeof( uint64_t ) * 8 - 1 ) >> 6 ) << 3;
                slab_cache.value( ).cache_pool->color_count  = slab_cache.value( ).cache_pool->free_count;
                slab_cache.value( ).cache_pool->color_map    = (uint64_t *)zone;
                std::memset( slab_cache.value( ).cache_pool->color_map, 0xff, slab_cache.value( ).cache_pool->color_length );
                zone = ( zone + slab_cache.value( ).cache_pool->color_length + sizeof( uint64_t ) * 10 ) & ( ~sizeof( uint64_t ) - 1 );
                for ( auto j = 0ul; j < slab_cache.value( ).cache_pool->color_count; ++j ) {
                    *( slab_cache.value( ).cache_pool->color_map + ( j >> 6 ) ) ^= 1ul << j % 64;
                }
                slab_cache.value( ).total_free                  = slab_cache.value( ).cache_pool->color_count;
                slab_cache.value( ).total_using                 = 0;
                slab_cache.value( ).cache_pool->page            = allocator_traits< decltype( page_allocator ) >::allocate( page_allocator, 1 );
                slab_cache.value( ).cache_pool->virtual_address = physical_to_virtual( slab_cache.value( ).cache_pool->page );
                slab_cache.value( ).cache_pool->list.container  = slab_cache.value( ).cache_pool;
                slab_cache.value( ).pool_list.append( slab_cache.value( ).cache_pool->list );
            } );
    }
}
}     // namespace QuantumNEC::Kernel