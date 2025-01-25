#include <kernel/memory/heap/kheap/kheap_allocater.hpp>
#include <kernel/memory/heap/slab/slab_allocater.hpp>
#include <kernel/memory/heap/slab/slab_creater.hpp>
#include <kernel/memory/memory.hpp>
#include <kernel/memory/page/page_allocater.hpp>
namespace QuantumNEC::Kernel {
inline PageAllocator< MemoryPageType::PAGE_2M > page_allocator { };

auto SlabAllocater::allocate( IN SlabCache *slab_cache, IN uint64_t arg ) -> void * {
    auto slab { slab_cache->cache_pool };
    auto set_slab = [ &slab, &slab_cache, &arg ]( uint64_t &i ) -> void * {
        if ( !( *( slab->color_map + ( i >> 6 ) ) & ( 1ul << ( i % 64 ) ) ) ) {
            *( slab->color_map + ( i >> 6 ) ) |= 1ul << ( i % 64 );
            slab->using_count++;
            slab->free_count--;

            slab_cache->total_free--;
            slab_cache->total_using++;
            if ( slab_cache->constructor ) {
                return slab_cache->constructor( (void *)( (uint64_t)slab->virtual_address + slab_cache->size * i ), arg );
            }
            else {
                return (void *)( (uint64_t)slab->virtual_address + slab_cache->size * i );
            }
        }
        return NULL;
    };
    KHeapAllocator< Slab >     kheap_allocator { };
    KHeapAllocator< uint64_t > color_map_allocator { };
    if ( !slab_cache->total_free ) {
        slab = std::allocator_traits< decltype( kheap_allocator ) >::allocate( kheap_allocator, 1 );

        if ( !slab ) {
            return NULL;
        }
        slab->list = *slab;

        slab->page = std::allocator_traits< decltype( page_allocator ) >::allocate( page_allocator, 1 );

        if ( !slab->page ) {
            std::allocator_traits< decltype( kheap_allocator ) >::deallocate( kheap_allocator, slab, 1 );
            return NULL;
        }
        std::memset( slab->page, 0, PageAllocator< MemoryPageType::PAGE_2M >::__page_size__ );
        slab->using_count = PageAllocator< MemoryPageType::PAGE_2M >::__page_size__ / slab_cache->size;
        slab->color_count = slab->using_count;
        slab->free_count  = slab->color_count;

        slab->virtual_address = physical_to_virtual( slab->page );
        slab->color_length    = ( ( slab->color_count + sizeof( uint64_t ) * 8 - 1 ) >> 6 ) << 3;
        slab->color_map       = std::allocator_traits< decltype( color_map_allocator ) >::allocate( color_map_allocator, slab->color_length / sizeof( uint64_t ) );

        if ( !slab->color_map ) {
            std::allocator_traits< decltype( page_allocator ) >::deallocate( page_allocator, slab->page, 1 );
            std::allocator_traits< decltype( kheap_allocator ) >::deallocate( kheap_allocator, slab, 1 );
            return NULL;
        }
        slab_cache->pool_list.insert( &slab->list, &slab_cache->cache_pool->list );
        slab_cache->total_free += slab->color_count;
        for ( auto i = 0ul; i < slab->color_count; ++i ) {
            if ( auto pool = set_slab( i ); pool ) {
                return pool;
            }
        }
    }
    else {
        for ( auto &slab : slab_cache->pool_list ) {
            if ( slab.free_count ) {
                void *pool { };
                for ( auto i = 0ul; i < slab.color_count; ++i ) {
                    if ( *( slab.color_map + ( i >> 6 ) ) == ~0ul ) {
                        i += 63;
                        continue;
                    }

                    pool = set_slab( i );
                }
                return pool;     // pool 必然不可能为NULL
            }
        }
    }

    if ( !slab ) {
        slab_cache->pool_list.remove( slab->list );
        std::allocator_traits< decltype( color_map_allocator ) >::deallocate( color_map_allocator, slab->color_map, slab->color_length / sizeof( uint64_t ) );
        std::allocator_traits< decltype( page_allocator ) >::deallocate( page_allocator, slab->page, 1 );
        std::allocator_traits< decltype( kheap_allocator ) >::deallocate( kheap_allocator, slab, 1 );
    }
    return NULL;
}
}     // namespace QuantumNEC::Kernel