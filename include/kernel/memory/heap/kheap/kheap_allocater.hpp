#pragma once
#include <cstddef>
#include <kernel/memory/page/page_allocater.hpp>
#include <lib/Uefi.hpp>
namespace QuantumNEC::Kernel {

template < typename T >
class KHeapAllocator {
public:
    using size_type                              = size_t;
    using difference_type                        = std::ptrdiff_t;
    using propagate_on_container_move_assignment = std::true_type;
    using self                                   = KHeapAllocator< T >;
    using type                                   = T;

public:
    explicit KHeapAllocator( void ) noexcept = default;
    ~KHeapAllocator( void ) noexcept         = default;

public:
    auto allocate( size_type size ) -> T * {
        size *= sizeof( T );
        if ( size > 1_MB ) /* 允许的最大分配数是1MB */ {
            return NULL;
        }

        auto result = KHeapManager::traversal_to_get_slab( size );

        PageAllocator< MemoryPageType::PAGE_2M > page_allocator { };
        if ( result.has_value( ) ) {
            auto &slab_cache = *result.value( );
            return slab_cache.visit( [ &, this ]( const Lib::shared_spinlock< SlabCache > &slab_cache ) -> T * {
                auto slab = slab_cache.value( ).cache_pool;

                if ( slab_cache.value( ).total_free ) {
                    for ( auto &it : slab_cache.value( ).pool_list ) {
                        if ( it.free_count ) {
                            slab = &it;
                            break;
                        }
                    }
                }
                else {
                    if ( slab = [ &page_allocator, &slab_cache, this ] -> Slab * {
                             auto page = std::allocator_traits< decltype( page_allocator ) >::allocate( page_allocator, 1 );

                             if ( !page ) {
                                 return NULL;
                             }
                             Slab *slab { };
                             switch ( slab_cache.value( ).size ) {
                             case 32:
                             case 64:
                             case 128:
                             case 256:
                             case 512: {
                                 auto virtual_address  = (uint64_t)physical_to_virtual( page );
                                 auto struct_size      = sizeof( Slab ) + PageAllocator< MemoryPageType::PAGE_2M >::__page_size__ / slab_cache.value( ).size / 8;
                                 slab                  = (Slab *)( virtual_address + PageAllocator< MemoryPageType::PAGE_2M >::__page_size__ - struct_size );
                                 slab->color_map       = (uint64_t *)slab + sizeof( Slab ) / sizeof( uint64_t );
                                 slab->free_count      = ( PageAllocator< MemoryPageType::PAGE_2M >::__page_size__ - ( PageAllocator< MemoryPageType::PAGE_2M >::__page_size__ / slab_cache.value( ).size / 8 ) - sizeof( Slab ) / slab_cache.value( ).size );
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
                                 slab->free_count      = PageAllocator< MemoryPageType::PAGE_2M >::__page_size__ / slab_cache.value( ).size;
                                 slab->using_count     = 0;
                                 slab->color_count     = slab->free_count;
                                 slab->color_length    = ( ( slab->color_count + sizeof( uint64_t ) * 8 - 1 ) >> 6 ) << 3;
                                 slab->color_map       = (uint64_t *)this->allocate( slab->color_length );
                                 slab->virtual_address = (void *)physical_to_virtual( page );
                                 slab->page            = (void *)page;

                             } break;
                             default:
                                 std::allocator_traits< decltype( page_allocator ) >::deallocate( page_allocator, page, 1 );
                                 return NULL;
                             }
                             std::memset( slab->color_map, 0xff, slab->color_length );
                             for ( auto i = 0ul; i < slab->color_count; ++i ) {
                                 *( slab->color_map + ( 1 >> 6 ) ) ^= 1ul << i % 64;
                             };
                             return ( slab );
                         }( );
                         !slab ) {
                        return NULL;
                    }
                    slab_cache.value( ).total_free += slab->color_count;
                    slab_cache.value( ).pool_list.insert( &slab->list, &slab_cache.value( ).cache_pool->list );
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
                        slab_cache.value( ).total_free--;
                        slab_cache.value( ).total_using++;
                        auto virtual_address = (T *)( (uint64_t)slab->virtual_address + slab_cache.value( ).size * i );
                        std::memset( virtual_address, 0, size );
                        return virtual_address;
                    }
                }
                return NULL;
            } );
        }
        if ( result.error( ) == KHeapManager::ErrorCode::CanNotFindSuitableSlabCache ) {
            // TODO 错误处理
        }
        return NULL;
    }
    auto allocate_at_least( size_type size ) -> T * {
        return this->allocate( size );
    }
    auto deallocate( T *address, [[maybe_unused]] size_type size ) -> void {
        auto page_base_address = PageAllocator< MemoryPageType::PAGE_2M >::__page_base__( address );
        auto result            = KHeapManager::traversal_to_find_page_base( page_base_address );

        PageAllocator< MemoryPageType::PAGE_2M > page_allocator { };
        if ( result.has_value( ) ) {
            auto [ slab, slab_cache ] = result.value( );
            if ( !slab ) {
                return;
            }
            slab_cache->visit( [ & ]( const Lib::shared_spinlock< SlabCache > &slab_cache ) {
                auto index = ( (uint64_t)address - (uint64_t)slab->virtual_address ) / slab_cache.value( ).size;
                *( slab->color_map + ( index >> 6 ) ) ^= 1ul << index % 64;     // 取消填充
                slab->free_count++;
                slab->using_count--;
                slab_cache.value( ).total_free++;
                slab_cache.value( ).total_using--;
                if ( !slab->using_count && ( slab_cache.value( ).total_free >= slab->color_count * 3 / 2 ) && ( slab_cache.value( ).cache_pool != slab ) ) {
                    slab_cache.value( ).pool_list.remove( slab->list );
                    slab_cache.value( ).total_free -= slab->color_count;
                    switch ( slab_cache.value( ).size ) {
                    case 32:
                    case 64:
                    case 128:
                    case 256:
                    case 512:
                        std::allocator_traits< decltype( page_allocator ) >::deallocate( page_allocator, slab->page, 1 );
                        break;
                    default:
                        KHeapAllocator< uint64_t > { }.deallocate( slab->color_map, slab->color_count );
                        std::allocator_traits< decltype( page_allocator ) >::deallocate( page_allocator, slab->page, 1 );
                        KHeapAllocator< Slab > { }.deallocate( slab, slab->color_count );
                        break;
                    }
                }
            } );
        }
    }
};
template < typename T >
inline auto ___kheap_allocate__( size_t size ) -> T * {
    return KHeapAllocator< T > { }.allocate( size );
}
template < typename T >
inline auto ___kheap_deallocate__( T *p, size_t size ) -> void {
    KHeapAllocator< T > { }.deallocate( p, size );
}
}     // namespace QuantumNEC::Kernel