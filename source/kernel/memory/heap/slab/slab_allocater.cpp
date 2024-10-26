#include <kernel/memory/heap/slab/slab_allocater.hpp>
#include <kernel/memory/heap/slab/slab_creater.hpp>
#include <kernel/memory/heap/heap_allocater.hpp>
#include <kernel/memory/heap/heap_collector.hpp>
#include <kernel/memory/page/page_allocater.hpp>
#include <kernel/memory/page/page_collector.hpp>
#include <kernel/memory/arch/memory_arch.hpp>
PUBLIC namespace QuantumNEC::Kernel {
    auto SlabAllocater::allocate( IN SlabCache * slab_cache, IN uint64_t arg ) -> VOID * {
        Slab *slab { };
        if ( !slab_cache->total_free ) {
            slab = reinterpret_cast< Slab * >( HeapAllocater { }.allocate( sizeof( Slab ) ) );
            if ( !slab ) {
                return NULL;
            }
            slab->list.container = slab;

            slab->page = PageAllocater { }.allocate< MemoryPageType::PAGE_2M >( 1 );
            if ( !slab->page ) {
                HeapCollector { }.free( slab );
                return NULL;
            }
            slab->color_count =
                slab->free_count =
                    slab->using_count =
                        PageAllocater::__page_size< MemoryPageType::PAGE_2M > / slab_cache->size;
            slab->virtual_address = physical_to_virtual( slab->page );
            slab->color_length = ( ( slab->color_count + sizeof( uint64_t ) * 8 - 1 ) >> 6 ) << 3;
            slab_cache->cache_pool->color_map = reinterpret_cast< uint64_t * >( HeapAllocater { }.allocate( slab_cache->cache_pool->color_length ) );

            if ( !slab->color_map ) {
                PageCollector { }.free< MemoryPageType::PAGE_2M >( slab_cache->cache_pool->page, 1 );
                HeapCollector { }.free( slab );
                return NULL;
            }
            slab_cache->pool_list.append( slab->list );
            slab_cache->total_free += slab->color_count;
        }
        auto i = 0ul;
        for ( ; i < slab->color_count; ++i ) {
            if ( !( *( slab->color_map + ( i >> 6 ) ) & ( 1ul << ( i % 64 ) ) ) ) {
                *( slab->color_map + ( i >> 6 ) ) |= 1ul << ( i % 64 );
                slab->using_count++;
                slab->free_count--;

                slab_cache->total_free--;
                slab_cache->total_using++;
            }
        }
    }
}