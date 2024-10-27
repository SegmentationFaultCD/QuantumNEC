#include <kernel/memory/heap/heap_allocater.hpp>
#include <kernel/memory/heap/heap_collector.hpp>
#include <kernel/memory/heap/slab/slab_destoryer.hpp>
#include <kernel/memory/page/page_allocater.hpp>
#include <kernel/memory/page/page_collector.hpp>
PUBLIC namespace QuantumNEC::Kernel {
    auto SlabDestroyer::destory( SlabCache * slab_cache ) -> VOID {
        if ( slab_cache->total_using ) {
            return;
        }
        // 遍历销毁SLAB内存池
        while ( slab_cache->pool_list.is_empty( ) ) {
            auto slab = reinterpret_cast< Slab * >( slab_cache->pool_list.pop( )->container );
            HeapCollector { }.free( slab->color_map );
            PageCollector { }.free< MemoryPageType::PAGE_2M >( slab->page, 1 );
            HeapCollector { }.free( slab );
        }
    }
}
