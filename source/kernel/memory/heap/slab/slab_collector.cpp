#include <kernel/memory/heap/kheap/kheap_walker.hpp>
#include <kernel/memory/heap/slab/slab_collector.hpp>
#include <kernel/memory/page/page_walker.hpp>
namespace QuantumNEC::Kernel {
inline PageWalker  page_walker { };
inline KHeapWalker kheap_walker { };

auto SlabCollector::free( IN SlabCache *slab_cache, IN void *address, IN uint64_t arg ) -> void {
    auto slab = slab_cache->cache_pool;

    for ( auto &it : slab_cache->pool_list ) {
        if ( it.virtual_address <= address && address < (void *)((uint64_t)( it.virtual_address ) + PageAllocater::__page_size__< MemoryPageType::PAGE_2M >)) {
            auto index = ( (uint64_t)address - (uint64_t)slab->virtual_address ) / slab_cache->size;
            *( slab->color_map + ( index >> 6 ) ) ^= 1ul << index % 64;
            slab->free_count++;
            slab->using_count--;
            slab_cache->total_free++;
            slab_cache->total_using--;
            if ( slab_cache->destructor ) {
                slab_cache->destructor( (void *)( (uint64_t)slab->virtual_address + slab_cache->size * index ), arg );
            }
            if ( !slab->using_count && slab_cache->total_free >= slab->color_count * 3 / 2 ) {
                slab_cache->pool_list.remove( slab->list );
                slab_cache->total_free -= slab->color_count;
                kheap_walker.free( slab->color_map );
                page_walker.free< MemoryPageType::PAGE_2M >( slab->page, 1 );
                kheap_walker.free( slab );
            }
            return;
        }
    }
}
}     // namespace QuantumNEC::Kernel