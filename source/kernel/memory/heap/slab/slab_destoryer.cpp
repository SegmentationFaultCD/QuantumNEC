#include <kernel/memory/heap/kheap/kheap_allocater.hpp>
#include <kernel/memory/heap/slab/slab_destoryer.hpp>
#include <kernel/memory/memory.hpp>
#include <kernel/memory/page/page_allocater.hpp>
namespace QuantumNEC::Kernel {
inline PageAllocator< MemoryPageType::PAGE_2M > page_allocater { };

auto SlabDestroyer::destory( SlabCache *slab_cache ) -> void {
    if ( slab_cache->total_using ) {
        return;
    }

    KHeapAllocator< Slab >     slab_allocator { };
    KHeapAllocator< uint64_t > color_map_allocator { };
    // 遍历销毁SLAB内存池

    while ( slab_cache->pool_list.is_empty( ) ) {
        auto slab = reinterpret_cast< Slab * >( slab_cache->pool_list.pop( )->container );
        allocator_traits< decltype( color_map_allocator ) >::deallocate( color_map_allocator, slab->color_map, slab->color_length / sizeof( uint64_t ) );
        allocator_traits< decltype( page_allocater ) >::deallocate( page_allocater, slab->page, 1 );
        allocator_traits< decltype( slab_allocator ) >::deallocate( slab_allocator, slab, 1 );
    }
}
}     // namespace QuantumNEC::Kernel
