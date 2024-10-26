#include <kernel/memory/heap/slab/slab_creater.hpp>
#include <kernel/memory/heap/heap_allocater.hpp>
#include <kernel/memory/heap/heap_collector.hpp>
#include <kernel/memory/page/page_allocater.hpp>
#include <kernel/memory/page/page_collector.hpp>
#include <lib/deflib.hpp>
#include <kernel/memory/arch/memory_arch.hpp>
PUBLIC namespace QuantumNEC::Kernel {
    auto SlabCreater::create( uint64_t size, IN uint64_t arg ) -> SlabCache * {
        auto slab_cache = reinterpret_cast< SlabCache * >( HeapAllocater { }.allocate( sizeof( SlabCache ) ) );
        if ( !slab_cache ) {
            // TODO: 错误处理
            return NULL;
        }
        slab_cache->size = Lib::SIZEOF_LONG_ALIGN( size );
        slab_cache->total_using = 0;
        slab_cache->total_free = 0;
        slab_cache->cache_pool = reinterpret_cast< Slab * >( HeapAllocater { }.allocate( sizeof( Slab ) ) );
        if ( !slab_cache->cache_pool ) {
            HeapCollector { }.free( slab_cache );
            return NULL;
        }
        slab_cache->cache_pool->list.init( );
        slab_cache->cache_dma_pool = NULL;
        slab_cache->cache_pool->page = PageAllocater { }.allocate< MemoryPageType::PAGE_2M >( 1 );
        if ( !slab_cache->cache_pool->page ) {
            HeapCollector { }.free( slab_cache->cache_pool );
            HeapCollector { }.free( slab_cache );
            return NULL;
        }
        slab_cache->cache_pool->color_count =
            slab_cache->total_free =
                slab_cache->cache_pool->free_count =
                    slab_cache->cache_pool->using_count =
                        slab_cache->cache_pool->using_count =
                            PageAllocater::__page_size< MemoryPageType::PAGE_2M > / slab_cache->size;
        slab_cache->cache_pool->virtual_address = physical_to_virtual( slab_cache->cache_pool->page );

        slab_cache->cache_pool->color_length = ( ( slab_cache->cache_pool->color_count + sizeof( uint64_t ) * 8 - 1 ) >> 6 ) << 3;
        slab_cache->cache_pool->color_map = reinterpret_cast< uint64_t * >( HeapAllocater { }.allocate( slab_cache->cache_pool->color_length ) );
        
        if ( !slab_cache->cache_pool->color_map ) {
            PageCollector { }.free< MemoryPageType::PAGE_2M >( slab_cache->cache_pool->page, 1 );
            HeapCollector { }.free( slab_cache->cache_pool );
            HeapCollector { }.free( slab_cache );
            return NULL;
        }

        return slab_cache;
    }
}