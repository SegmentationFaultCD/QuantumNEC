#include <kernel/memory/heap/kheap/kheap_manager.hpp>
#include <kernel/memory/heap/kheap/kheap_walker.hpp>
#include <kernel/memory/heap/slab/slab.hpp>
#include <kernel/memory/page/page_walker.hpp>
PUBLIC namespace QuantumNEC::Kernel {
    inline PageWalker  page_walker { };
    inline KHeapWalker kheap_walker { };

    auto KHeapCollector::free( IN VOID * address ) -> VOID {
        auto page_base_address = PageWalker::__page_base__< MemoryPageType::PAGE_2M >( address );

        auto result = KHeapManager::traversal_to_find_page_base( page_base_address );
        if ( result.has_value( ) ) {
            auto [ slab, slab_cache ] = result.value( );
            if ( !slab ) {
                return;
            }
            auto index = ( (uint64_t)address - (uint64_t)slab->virtual_address ) / slab_cache->size;
            *( slab->color_map + ( index >> 6 ) ) ^= 1ul << index % 64;     // 取消填充
            slab->free_count++;
            slab->using_count--;
            slab_cache->total_free++;
            slab_cache->total_using--;
            if ( !slab->using_count && ( slab_cache->total_free >= slab->color_count * 3 / 2 ) && ( slab_cache->cache_pool != slab ) ) {
                slab_cache->pool_list.remove( slab->list );
                slab_cache->total_free -= slab->color_count;
                switch ( slab_cache->size ) {
                case 32:
                case 64:
                case 128:
                case 256:
                case 512:
                    page_walker.free< MemoryPageType::PAGE_2M >( slab->page, 1 );
                    break;
                default:
                    kheap_walker.free( slab->color_map );
                    page_walker.free< MemoryPageType::PAGE_2M >( slab->page, 1 );
                    kheap_walker.free( slab );
                    break;
                }
            }
        }
    }
}
