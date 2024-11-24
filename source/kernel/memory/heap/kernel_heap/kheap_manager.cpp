#include <kernel/memory/arch/memory_arch.hpp>
#include <kernel/memory/heap/kheap/kheap_manager.hpp>
#include <kernel/memory/heap/slab/slab.hpp>
#include <kernel/memory/page/page_walker.hpp>
#include <kernel/print.hpp>
#include <libcxx/string.hpp>
PUBLIC namespace QuantumNEC::Kernel {
    inline PageWalker page_walker { };

    KHeapManager::KHeapManager( ) noexcept {
        auto zone = (uint64_t)physical_to_virtual( page_walker.allocate< MemoryPageType::PAGE_2M >( 12 ) );
        for ( auto i = 0ul; i < this->cache_size_count; ++i ) {
            this->slab_caches[ i ].pool_list.init( );
            this->slab_caches[ i ].size       = this->cache_size[ i ];
            this->slab_caches[ i ].cache_pool = new ( (void *)zone ) Slab { };
            zone += sizeof( Slab ) + sizeof( uint64_t ) * 10;
            this->slab_caches[ i ].cache_pool->using_count  = 0;
            this->slab_caches[ i ].cache_pool->free_count   = PageWalker::__page_size__< MemoryPageType::PAGE_2M > / this->cache_size[ i ];
            this->slab_caches[ i ].cache_pool->color_length = ( ( PageWalker::__page_size__< MemoryPageType::PAGE_2M > / this->cache_size[ i ] + sizeof( uint64_t ) * 8 - 1 ) >> 6 ) << 3;
            this->slab_caches[ i ].cache_pool->color_count  = this->slab_caches[ i ].cache_pool->free_count;
            this->slab_caches[ i ].cache_pool->color_map    = new ( (void *)zone ) uint64_t[ this->slab_caches[ i ].cache_pool->color_length ] { 0xff };
            zone                                            = ( zone + this->slab_caches[ i ].cache_pool->color_length + sizeof( uint64_t ) * 10 ) & ( ~sizeof( uint64_t ) - 1 );
            for ( auto j = 0ul; j < this->slab_caches[ i ].cache_pool->color_count; ++j ) {
                *( this->slab_caches[ i ].cache_pool->color_map + ( j >> 6 ) ) ^= 1ul << j % 64;
            }
            this->slab_caches[ i ].total_free                  = this->slab_caches[ i ].cache_pool->color_count;
            this->slab_caches[ i ].total_using                 = 0;
            this->slab_caches[ i ].cache_pool->page            = page_walker.allocate< MemoryPageType::PAGE_2M >( 1 );
            this->slab_caches[ i ].cache_pool->virtual_address = physical_to_virtual( this->slab_caches[ i ].cache_pool->page );
            this->slab_caches[ i ].cache_pool->list.container  = this->slab_caches[ i ].cache_pool;
            this->slab_caches[ i ].pool_list.append( this->slab_caches[ i ].cache_pool->list );
        }
    }
}