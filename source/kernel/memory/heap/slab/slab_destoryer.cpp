#include <kernel/memory/arch/memory_arch.hpp>
#include <kernel/memory/heap/kheap/kheap_walker.hpp>
#include <kernel/memory/heap/slab/slab_destoryer.hpp>
#include <kernel/memory/page/page_walker.hpp>
PUBLIC namespace QuantumNEC::Kernel {
    inline PageWalker  page_walker { };
    inline KHeapWalker kheap_walker { };

    auto SlabDestroyer::destory( SlabCache * slab_cache ) -> VOID {
        if ( slab_cache->total_using ) {
            return;
        }
        // 遍历销毁SLAB内存池
        while ( slab_cache->pool_list.is_empty( ) ) {
            auto slab = reinterpret_cast< Slab * >( slab_cache->pool_list.pop( )->container );
            kheap_walker.free( slab->color_map );
            page_walker.free< MemoryPageType::PAGE_2M >( slab->page, 1 );
            kheap_walker.free( slab );
        }
    }
}
