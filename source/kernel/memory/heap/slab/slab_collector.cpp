#include <kernel/memory/heap/kheap/kheap_allocater.hpp>
#include <kernel/memory/heap/slab/slab_collector.hpp>
#include <kernel/memory/page/page_allocater.hpp>
namespace QuantumNEC::Kernel {
inline PageAllocator< MemoryPageType::PAGE_2M > page_allocator { };

auto SlabCollector::free( IN SlabCache *slab_cache, IN void *address, IN uint64_t arg ) -> void {
    auto slab = slab_cache->cache_pool;

    KHeapAllocator< Slab >     kheap_allocator { };
    KHeapAllocator< uint64_t > color_map_allocator { };

    for ( auto &it : slab_cache->pool_list ) {
        if ( it.virtual_address <= address && address < (void *)( (uint64_t)( it.virtual_address ) + PageAllocator< MemoryPageType::PAGE_2M >::__page_size__ ) ) {
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
                allocator_traits< decltype( color_map_allocator ) >::deallocate( color_map_allocator, slab->color_map, 1 );
                allocator_traits< decltype( page_allocator ) >::deallocate( page_allocator, slab->page, 1 );
                allocator_traits< decltype( kheap_allocator ) >::deallocate( kheap_allocator, slab, 1 );
            }
            return;
        }
    }
}
}     // namespace QuantumNEC::Kernel