#include <kernel/memory/heap/kheap/kheap_manager.hpp>
#include <kernel/memory/heap/kheap/kheap_walker.hpp>
#include <kernel/memory/heap/slab/slab.hpp>
#include <kernel/memory/memory.hpp>
#include <kernel/memory/page/page_manager.hpp>
#include <kernel/memory/page/page_walker.hpp>
#include <kernel/print.hpp>
namespace QuantumNEC::Kernel {
auto KHeapAllocater::allocate( uint64_t size ) -> void * {
    if ( size > 1_MB ) /* 允许的最大分配数是1MB */ {
        return NULL;
    }
    auto result = KHeapManager::traversal_to_get_slab( size );

    if ( result.has_value( ) ) {
        auto &slab_cache = *result.value( );

        auto slab = slab_cache.cache_pool;

        if ( slab_cache.total_free ) {
            for ( auto &it : slab_cache.pool_list ) {
                if ( it.free_count ) {
                    slab = &it;
                    break;
                }
            }
        }
        else {
            if ( slab = [ &slab_cache, this ] -> Slab * {
                     auto page = PageWalker { }.allocate< MemoryPageType::PAGE_2M >( 1 );

                     if ( !page ) {
                         return NULL;
                     }
                     Slab *slab { };
                     switch ( slab_cache.size ) {
                     case 32:
                     case 64:
                     case 128:
                     case 256:
                     case 512: {
                         auto virtual_address  = (uint64_t)physical_to_virtual( page );
                         auto struct_size      = sizeof( Slab ) + PageWalker::__page_size__< MemoryPageType::PAGE_2M > / slab_cache.size / 8;
                         slab                  = (Slab *)( virtual_address + PageWalker::__page_size__< MemoryPageType::PAGE_2M > - struct_size );
                         slab->color_map       = (uint64_t *)slab + sizeof( Slab ) / sizeof( uint64_t );
                         slab->free_count      = ( PageWalker::__page_size__< MemoryPageType::PAGE_2M > - ( PageWalker::__page_size__< MemoryPageType::PAGE_2M > / slab_cache.size / 8 ) - sizeof( Slab ) / slab_cache.size );
                         slab->using_count     = 0;
                         slab->color_count     = slab->free_count;
                         slab->virtual_address = (void *)virtual_address;
                         slab->page            = (void *)page;
                         slab->color_length    = ( ( slab->color_count + sizeof( uint64_t ) * 8 - 1 ) >> 6 << 3 );
                     } break;
                     case 1024:
                     case 2048:
                     case 4096:
                     case 8192:
                     case 16384:
                     case 32768:
                     case 65536:
                     case 131072:
                     case 262144:
                     case 524288:
                     case 1048576: {
                         slab                  = (Slab *)this->allocate( sizeof( Slab ) );
                         slab->free_count      = PageWalker::__page_size__< MemoryPageType::PAGE_2M > / slab_cache.size;
                         slab->using_count     = 0;
                         slab->color_count     = slab->free_count;
                         slab->color_length    = ( ( slab->color_count + sizeof( uint64_t ) * 8 - 1 ) >> 6 ) << 3;
                         slab->color_map       = (uint64_t *)this->allocate( slab->color_length );
                         slab->virtual_address = (void *)physical_to_virtual( page );
                         slab->page            = (void *)page;

                     } break;
                     default:
                         PageWalker { }.free< MemoryPageType::PAGE_2M >( page, 1 );
                         return NULL;
                     }
                     std::memset( slab->color_map, 0xff, slab->color_length );
                     for ( auto i = 0ul; i < slab->color_count; ++i ) {
                         *( slab->color_map + ( 1 >> 6 ) ) ^= 1ul << i % 64;
                     };
                     return slab;
                 }( );
                 !slab ) {
                return NULL;
            }
            slab_cache.total_free += slab->color_count;
            slab_cache.pool_list.insert( &slab->list, &slab_cache.cache_pool->list );
        }
        for ( auto i = 0ul; i < slab->color_count; ++i ) {
            if ( *( slab->color_map + ( i >> 6 ) ) == ~0ul ) {
                i += 63;
                continue;
            }
            if ( !( *( slab->color_map + ( i >> 6 ) ) & ( 1ul << ( i % 64 ) ) ) ) {
                *( slab->color_map + ( i >> 6 ) ) |= 1ul << ( i % 64 );
                slab->using_count++;
                slab->free_count--;
                slab_cache.total_free--;
                slab_cache.total_using++;
                auto virtual_address = (void *)( (uint64_t)slab->virtual_address + slab_cache.size * i );
                std::memset( virtual_address, 0, size );
                return virtual_address;
            }
        }
        return NULL;
    }
    if ( result.error( ) == KHeapManager::ErrorCode::CanNotFindSuitableSlabCache ) {
        // TODO 错误处理
    }
    return NULL;
}
}     // namespace QuantumNEC::Kernel