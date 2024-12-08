#include <kernel/memory/heap/kheap/kheap_walker.hpp>
#include <kernel/memory/heap/slab/slab_collector.hpp>
#include <kernel/memory/heap/slab/slab_creater.hpp>
#include <kernel/memory/memory.hpp>
#include <kernel/memory/page/page_walker.hpp>
#include <lib/deflib.hpp>
namespace QuantumNEC::Kernel {
inline PageWalker  page_walker { };
inline KHeapWalker kheap_walker { };

auto SlabCreater::create( uint64_t size, IN FuncPtr< void *, void *, uint64_t > constructor, IN FuncPtr< void *, void *, uint64_t > destructor ) -> SlabCache * {
    auto slab_cache = new ( kheap_walker.allocate( sizeof( SlabCache ) ) ) SlabCache { };
    if ( !slab_cache ) {
        // TODO: 错误处理
        return NULL;
    }
    slab_cache->size        = Lib::SIZEOF_LONG_ALIGN( size );
    slab_cache->total_using = 0;
    slab_cache->total_free  = 0;
    slab_cache->cache_pool  = new ( kheap_walker.allocate( sizeof( Slab ) ) ) Slab { };
    slab_cache->constructor = constructor;
    slab_cache->destructor  = destructor;
    if ( !slab_cache->cache_pool ) {
        kheap_walker.free( slab_cache );
        return NULL;
    }

    slab_cache->pool_list.append( slab_cache->cache_pool->list );
    slab_cache->cache_pool->list.container = slab_cache->cache_pool;

    slab_cache->cache_dma_pool   = NULL;
    slab_cache->cache_pool->page = page_walker.allocate< MemoryPageType::PAGE_2M >( 1 );
    if ( !slab_cache->cache_pool->page ) {
        kheap_walker.free( slab_cache->cache_pool );
        kheap_walker.free( slab_cache );
        return NULL;
    }
    std::memset( slab_cache->cache_pool->page, 0, PageWalker::__page_size__< MemoryPageType::PAGE_2M > );
    slab_cache->cache_pool->color_count = PageWalker::__page_size__< MemoryPageType::PAGE_2M > / slab_cache->size;
    slab_cache->cache_pool->using_count = slab_cache->cache_pool->color_count;
    slab_cache->total_free              = slab_cache->cache_pool->using_count;
    slab_cache->cache_pool->free_count  = slab_cache->total_free;

    slab_cache->cache_pool->virtual_address = physical_to_virtual( slab_cache->cache_pool->page );

    slab_cache->cache_pool->color_length = ( ( slab_cache->cache_pool->color_count + sizeof( uint64_t ) * 8 - 1 ) >> 6 ) << 3;
    slab_cache->cache_pool->color_map    = reinterpret_cast< uint64_t    *>( kheap_walker.allocate( slab_cache->cache_pool->color_length ) );

    if ( !slab_cache->cache_pool->color_map ) {
        page_walker.free< MemoryPageType::PAGE_2M >( slab_cache->cache_pool->page, 1 );
        kheap_walker.free( slab_cache->cache_pool );
        kheap_walker.free( slab_cache );
        return NULL;
    }

    return slab_cache;
}
}     // namespace QuantumNEC::Kernel