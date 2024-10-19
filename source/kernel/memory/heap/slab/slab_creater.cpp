#include <kernel/memory/heap/slab/slab_creater.hpp>
#include <kernel/memory/heap/heap_allocater.hpp>
#include <lib/deflib.hpp>
PUBLIC namespace QuantumNEC::Kernel {
    auto SlabCreater::create( uint64_t size, IN uint64_t arg ) -> SlabCache * {
        auto slab_cache = reinterpret_cast< SlabCache * >( HeapAllocater { }.allocate( sizeof( SlabCache ) ) );
        if ( !slab_cache ) {
            // TODO: 错误处理
            while ( TRUE );
        }
        slab_cache->size = Lib::SIZEOF_LONG_ALIGN( size );
    }
}