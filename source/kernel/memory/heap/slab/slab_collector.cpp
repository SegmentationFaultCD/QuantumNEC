#include <kernel/memory/heap/kheap/kheap_allocater.hpp>
#include <kernel/memory/heap/kheap/kheap_collector.hpp>
#include <kernel/memory/heap/slab/slab_collector.hpp>
#include <kernel/memory/page/page_allocater.hpp>
#include <kernel/memory/page/page_collector.hpp>
PUBLIC namespace QuantumNEC::Kernel {
    auto SlabCollector::free( IN SlabCache * slab_cache, IN VOID * address, IN uint64_t arg ) -> VOID {
        auto slab = slab_cache->cache_pool;

        auto node = slab_cache->pool_list.traversal(
            []( Lib::ListNode *node, VOID *address ) -> BOOL {
                if ( ( (Slab *)node )->virtual_address <= address && address < (VOID *)((uint64_t)( ( (Slab *)node )->virtual_address ) + PageAllocater::__page_size__< MemoryPageType::PAGE_2M >)) {
                    return TRUE;
                }
                return FALSE;
            },
            address );
        if ( node ) {
            auto index = ( (uint64_t)address - (uint64_t)slab->virtual_address ) / slab_cache->size;
            *( slab->color_map + ( index >> 6 ) ) ^= 1ul << index % 64;
            slab->free_count++;
            slab->using_count--;
            slab_cache->total_free++;
            slab_cache->total_using--;
            if ( slab_cache->destructor ) {
                slab_cache->destructor( (VOID *)( (uint64_t)slab->virtual_address + slab_cache->size * index ), arg );
            }
            if ( !slab->using_count && slab_cache->total_free >= slab->color_count * 3 / 2 ) {
                slab_cache->pool_list.remove( slab->list );
                slab_cache->total_free -= slab->color_count;
                KHeapCollector { }.free( slab->color_map );
                PageCollector { }.free< MemoryPageType::PAGE_2M >( slab->page, 1 );
                KHeapCollector { }.free( slab );
            }
            return;
        }
        return;
    }
}