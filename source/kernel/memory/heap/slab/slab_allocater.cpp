#include <kernel/memory/arch/memory_arch.hpp>
#include <kernel/memory/heap/heap_allocater.hpp>
#include <kernel/memory/heap/heap_collector.hpp>
#include <kernel/memory/heap/slab/slab_allocater.hpp>
#include <kernel/memory/heap/slab/slab_creater.hpp>
#include <kernel/memory/page/page_allocater.hpp>
#include <kernel/memory/page/page_collector.hpp>
PUBLIC namespace QuantumNEC::Kernel {
    auto SlabAllocater::allocate( IN SlabCache * slab_cache, IN uint64_t arg ) -> VOID * {
        auto slab { slab_cache->cache_pool };
        auto set_slab = [ &slab, &slab_cache, &arg ]( uint64_t &i ) -> VOID * {
            if ( !( *( slab->color_map + ( i >> 6 ) ) & ( 1ul << ( i % 64 ) ) ) ) {
                *( slab->color_map + ( i >> 6 ) ) |= 1ul << ( i % 64 );
                slab->using_count++;
                slab->free_count--;

                slab_cache->total_free--;
                slab_cache->total_using++;
                if ( slab_cache->constructor ) {
                    return slab_cache->constructor( (VOID *)( (uint64_t)slab->virtual_address + slab_cache->size * i ), arg );
                }
                else {
                    return (VOID *)( (uint64_t)slab->virtual_address + slab_cache->size * i );
                }
            }
            return NULL;
        };
        if ( !slab_cache->total_free ) {
            slab = reinterpret_cast< Slab * >( HeapAllocater { }.allocate( sizeof( Slab ) ) );

            if ( !slab ) {
                return NULL;
            }
            slab->list.container = slab;

            slab->page = PageAllocater { }.allocate< MemoryPageType::PAGE_2M >( 1 );

            if ( !slab->page ) {
                HeapCollector { }.free( slab );
                return NULL;
            }
            std::memset( slab->page, 0, PageAllocater::__page_size__< MemoryPageType::PAGE_2M > );
            slab->using_count = PageAllocater::__page_size__< MemoryPageType::PAGE_2M > / slab_cache->size;
            slab->color_count = slab->using_count;
            slab->free_count  = slab->color_count;

            slab->virtual_address = physical_to_virtual( slab->page );
            slab->color_length    = ( ( slab->color_count + sizeof( uint64_t ) * 8 - 1 ) >> 6 ) << 3;
            slab->color_map       = reinterpret_cast< uint64_t       *>( HeapAllocater { }.allocate( slab->color_length ) );

            if ( !slab->color_map ) {
                PageCollector { }.free< MemoryPageType::PAGE_2M >( slab->page, 1 );
                HeapCollector { }.free( slab );
                return NULL;
            }
            slab_cache->pool_list.append( slab->list );
            slab_cache->total_free += slab->color_count;
            for ( auto i = 0ul; i < slab->color_count; ++i ) {
                if ( auto pool = set_slab( i ); pool ) {
                    return pool;
                }
            }
        }
        else {
            slab = (Slab *)( slab_cache->pool_list.traversal(
                                                      []( Lib::ListNode *node, uint64_t ) -> BOOL {
                                                          if ( ( (Slab *)node->container )->free_count ) {
                                                              return TRUE;
                                                          }
                                                          return FALSE;
                                                      },
                                                      0 )
                                 ->container );     // slab 必然不可能为NULL

            VOID *pool { };
            for ( auto i = 0ul; i < slab->color_count; ++i ) {
                if ( *( slab->color_map + ( i >> 6 ) ) == ~0ul ) {
                    i += 63;
                    continue;
                }

                pool = set_slab( i );
            }
            return pool;     // pool 必然不可能为NULL
        }
        if ( !slab ) {
            slab_cache->pool_list.remove( slab->list );
            HeapCollector { }.free( slab->color_map );
            PageCollector { }.free< MemoryPageType::PAGE_2M >( slab, 1 );
            HeapCollector { }.free( slab );
        }
        return NULL;
    }
}