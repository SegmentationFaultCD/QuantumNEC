#include <kernel/memory/arch/memory_arch.hpp>
#include <kernel/memory/heap/slab/slab.hpp>
#include <kernel/memory/page/page_allocater.hpp>
#include <libcxx/string.hpp>
PUBLIC namespace QuantumNEC::Kernel {
    SlabManager::SlabManager( VOID ) noexcept {
        auto zone = (uint64_t)physical_to_virtual( PageAllocater { }.allocate< MemoryPageType::PAGE_2M >( 4 ) );
        std::memset( (void *)zone, 0, PageAllocater::__page_size__< MemoryPageType::PAGE_2M > * 4 );
        for ( auto i = 0ul; i < this->cache_size_count; ++i ) {
            this->slab_caches[ i ].pool_list.init( );
            this->slab_caches[ i ].size       = this->cache_size[ i ];
            this->slab_caches[ i ].cache_pool = (Slab *)zone;
            zone += sizeof( Slab ) + sizeof( uint64_t ) * 10;
            this->slab_caches[ i ].cache_pool->using_count  = 0;
            this->slab_caches[ i ].cache_pool->free_count   = PageAllocater::__page_size__< MemoryPageType::PAGE_2M > / this->cache_size[ i ];
            this->slab_caches[ i ].cache_pool->color_length = ( ( PageAllocater::__page_size__< MemoryPageType::PAGE_2M > / this->cache_size[ i ] + sizeof( uint64_t ) * 8 - 1 ) >> 6 ) << 3;
            this->slab_caches[ i ].cache_pool->color_count  = this->slab_caches[ i ].cache_pool->free_count;
            this->slab_caches[ i ].cache_pool->color_map    = (uint64_t *)zone;
            zone += ( this->slab_caches[ i ].cache_pool->color_length + sizeof( uint64_t ) * 10 ) & ( ~sizeof( uint64_t ) - 1 );
            for ( auto i = 0ul; i < this->slab_caches[ i ].cache_pool->color_count; ++i ) {
                *( this->slab_caches[ i ].cache_pool->color_map + ( i >> 6 ) ) ^= 1ul << i % 64;
            }
            this->slab_caches[ i ].total_free                  = this->slab_caches[ i ].cache_pool->color_count;
            this->slab_caches[ i ].total_using                 = 0;
            this->slab_caches[ i ].cache_pool->page            = PageAllocater { }.allocate< MemoryPageType::PAGE_2M >( 1 );
            this->slab_caches[ i ].cache_pool->virtual_address = physical_to_virtual( this->slab_caches[ i ].cache_pool->page );
            std::memset( this->slab_caches[ i ].cache_pool->virtual_address, 0xff, PageAllocater::__page_size__< MemoryPageType::PAGE_2M > );
            this->slab_caches[ i ].pool_list.append( this->slab_caches[ i ].cache_pool->list );
        }
    }
}