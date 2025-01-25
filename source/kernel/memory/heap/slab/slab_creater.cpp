#include <kernel/memory/heap/kheap/kheap_allocater.hpp>
#include <kernel/memory/heap/slab/slab_collector.hpp>
#include <kernel/memory/heap/slab/slab_creater.hpp>
#include <kernel/memory/memory.hpp>
#include <kernel/memory/page/page_allocater.hpp>
#include <lib/deflib.hpp>
namespace QuantumNEC::Kernel {
inline PageAllocator< MemoryPageType::PAGE_2M > page_allocater { };

auto SlabCreater::create( uint64_t size, IN FuncPtr< void *, void *, uint64_t > constructor, IN FuncPtr< void *, void *, uint64_t > destructor ) -> SlabCache * {
    KHeapAllocator< SlabCache > slab_cache_allocator { };
    KHeapAllocator< Slab >      slab_allocator { };
    KHeapAllocator< uint64_t >  color_map_allocator { };

    auto slab_cache = std::allocator_traits< decltype( slab_cache_allocator ) >::allocate( slab_cache_allocator, 1 );
    std::allocator_traits< decltype( slab_cache_allocator ) >::construct( slab_cache_allocator, slab_cache );
    if ( !slab_cache ) {
        // TODO: 错误处理
        return NULL;
    }
    slab_cache->size        = Lib::SIZEOF_LONG_ALIGN( size );
    slab_cache->total_using = 0;
    slab_cache->total_free  = 0;
    slab_cache->cache_pool  = std::allocator_traits< decltype( slab_allocator ) >::allocate( slab_allocator, 1 );
    std::allocator_traits< decltype( slab_allocator ) >::construct( slab_allocator, slab_cache->cache_pool );
    slab_cache->constructor = constructor;
    slab_cache->destructor  = destructor;
    if ( !slab_cache->cache_pool ) {
        std::allocator_traits< decltype( slab_cache_allocator ) >::deallocate( slab_cache_allocator, slab_cache, 1 );
        return NULL;
    }

    slab_cache->pool_list.append( slab_cache->cache_pool->list );
    slab_cache->cache_pool->list = *slab_cache->cache_pool;

    slab_cache->cache_dma_pool   = NULL;
    slab_cache->cache_pool->page = std::allocator_traits< decltype( page_allocater ) >::allocate( page_allocater, 1 );

    if ( !slab_cache->cache_pool->page ) {
        std::allocator_traits< decltype( slab_allocator ) >::construct( slab_allocator, slab_cache->cache_pool );
        std::allocator_traits< decltype( slab_cache_allocator ) >::deallocate( slab_cache_allocator, slab_cache, 1 );
        return NULL;
    }
    std::memset( slab_cache->cache_pool->page, 0, PageAllocator< MemoryPageType::PAGE_2M >::__page_size__ );
    slab_cache->cache_pool->color_count     = PageAllocator< MemoryPageType::PAGE_2M >::__page_size__ / slab_cache->size;
    slab_cache->cache_pool->using_count     = slab_cache->cache_pool->color_count;
    slab_cache->total_free                  = slab_cache->cache_pool->using_count;
    slab_cache->cache_pool->free_count      = slab_cache->total_free;
    slab_cache->cache_pool->virtual_address = physical_to_virtual( slab_cache->cache_pool->page );
    slab_cache->cache_pool->color_length    = ( ( slab_cache->cache_pool->color_count + sizeof( uint64_t ) * 8 - 1 ) >> 6 ) << 3;
    slab_cache->cache_pool->color_map       = std::allocator_traits< decltype( color_map_allocator ) >::allocate( color_map_allocator, slab_cache->cache_pool->color_length / sizeof( uint64_t ) );
    if ( !slab_cache->cache_pool->color_map ) {
        std::allocator_traits< decltype( page_allocater ) >::deallocate( page_allocater, slab_cache->cache_pool->page, 1 );
        std::allocator_traits< decltype( slab_allocator ) >::construct( slab_allocator, slab_cache->cache_pool );
        std::allocator_traits< decltype( slab_cache_allocator ) >::deallocate( slab_cache_allocator, slab_cache, 1 );
        return NULL;
    }
    return slab_cache;
}
}     // namespace QuantumNEC::Kernel