#pragma once
#include <cstddef>
#include <kernel/memory/allocator/page.hpp>
#include <kernel/memory/allocator/slab.hpp>
#include <kernel/memory/paging/hhdm.hpp>
namespace Memory::KernelHeap {
auto initialize( void ) -> void;
template < typename T >
class allocator;
struct allocator_manager {
    template < typename T >
    friend class allocator;

    friend auto initialize( void ) -> void;

private:
    constexpr static std::uint64_t cache_size[] {
        32,
        64,
        128,
        256,
        512,
        1024,
        2048,
        4096,
        8192,
        16384,
        32768,
        65536,
        131072,
        262144,
        524288,
        1048576,
    };
    constexpr static auto cache_size_count = sizeof( cache_size ) / sizeof( uint64_t );
    inline static SlabCache slab_caches[ cache_size_count ];
};

template < typename T >
class allocator : public Memory::allocator< T > {
    friend auto initialize( void ) -> void;

public:
    using value_type = T;
    using pointer = value_type *;
    using const_pointer = const value_type *;
    using size_type = size_t;
    using difference_type = std::ptrdiff_t;
    using propagate_on_container_move_assignment = std::true_type;
    using self = allocator< T >;
    using type = T;

private:
    constexpr static std::uint64_t cache_size[] {
        32,
        64,
        128,
        256,
        512,
        1024,
        2048,
        4096,
        8192,
        16384,
        32768,
        65536,
        131072,
        262144,
        524288,
        1048576,
    };

public:
    constexpr static auto cache_size_count = sizeof( cache_size ) / sizeof( uint64_t );
    inline static Task::s_locks kheap_lock { };

public:
    allocator( void ) noexcept {}
    virtual ~allocator( void ) noexcept {}

    static auto traversal_to_get_slab( uint64_t size ) -> SlabCache * {
        for ( auto i = 0ul; i < cache_size_count; ++i ) {
            if ( allocator_manager::slab_caches[ i ].size >= size ) {
                return &allocator_manager::slab_caches[ i ];
            }
        }
        return nullptr;
    }

    static auto traversal_to_find_page_base( uint64_t page_base_address ) -> std::pair< Slab *, SlabCache * > {
        for ( auto i = 0ul; i < cache_size_count; ++i ) {
            for ( auto &slab : allocator_manager::slab_caches[ i ].pool_list ) {
                if ( (uint64_t)slab.virtual_address == page_base_address ) {
                    return { &slab, &allocator_manager::slab_caches[ i ] };
                }
            }
        }
        return { };
    }

public:
    virtual auto allocate( size_type size ) -> T * override {
        std::lock_guard guard { this->kheap_lock };
        auto addr = this->_allocate( size );
        return addr;
    }
    virtual auto _allocate( size_type size ) -> T * {
        using namespace Memory::Page;
        if constexpr ( !std::is_void_v< T > ) {
            size *= sizeof( T );
        }

        if ( size > 1_MB ) /* 允许的最大分配数是1MB */ {
            return nullptr;
        }

        auto slab_cache = traversal_to_get_slab( size );

        Page::allocator< Type::P2Mib > page_allocator { };

        if ( slab_cache != nullptr ) {
            auto slab = slab_cache->cache_pool;

            if ( slab_cache->total_free ) {
                for ( auto &it : slab_cache->pool_list ) {
                    if ( it.free_count ) {
                        slab = &it;
                        break;
                    }
                }
            }
            else {
                if ( slab = [ &page_allocator, &slab_cache, this ] -> Slab * {
                         auto page = page_allocator._allocate( 1 );

                         if ( !page ) {
                             return nullptr;
                         }
                         Slab *slab { };
                         switch ( slab_cache->size ) {
                         case 32:
                         case 64:
                         case 128:
                         case 256:
                         case 512: {
                             auto virtual_address = (uint64_t)physical_to_virtual( page );
                             auto struct_size = sizeof( Slab ) + page_allocator.__page_size__ / slab_cache->size / 8;
                             slab = (Slab *)( virtual_address + page_allocator.__page_size__ - struct_size );
                             slab->color_map = (uint64_t *)( (uint64_t)slab + sizeof( struct Slab ) );
                             slab->free_count = ( page_allocator.__page_size__ - ( page_allocator.__page_size__ / slab_cache->size / 8 ) - sizeof( Slab ) / slab_cache->size );
                             slab->using_count = 0;
                             slab->color_count = slab->free_count;
                             slab->virtual_address = (void *)virtual_address;
                             slab->page = (void *)page;
                             slab->color_length = ( ( slab->color_count + sizeof( uint64_t ) * 8 - 1 ) >> 6 << 3 );
                             std::construct_at( &slab->list );
                             std::memset( slab->color_map, 0xff, slab->color_length );
                             for ( auto i = 0ul; i < slab->color_count; i++ )
                                 *( slab->color_map + ( i >> 6 ) ) ^= 1UL << i % 64;
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
                             slab = (Slab *)this->_allocate( sizeof( Slab ) );
                             slab->free_count = page_allocator.__page_size__ / slab_cache->size;
                             slab->using_count = 0;
                             slab->color_count = slab->free_count;
                             slab->color_length = ( ( slab->color_count + sizeof( uint64_t ) * 8 - 1 ) >> 6 ) << 3;
                             slab->color_map = (uint64_t *)this->_allocate( slab->color_length );
                             slab->virtual_address = (void *)physical_to_virtual( page );
                             slab->page = (void *)page;
                             std::construct_at( &slab->list );
                             std::memset( slab->color_map, 0xff, slab->color_length );
                             for ( auto i = 0ul; i < slab->color_count; ++i ) {
                                 *( slab->color_map + ( 1 >> 6 ) ) ^= 1ul << i % 64;
                             };
                         } break;
                         default:
                             page_allocator._deallocate( page, 1 );

                             return nullptr;
                         }

                         return ( slab );
                     }( );
                     !slab ) {
                    return nullptr;
                }
                slab_cache->total_free += slab->color_count;
                slab_cache->pool_list.insert( &slab->list, &slab_cache->cache_pool->list );
            }

            for ( auto i = 0ul; i < slab->color_count; ++i ) {
                if ( *( slab->color_map + ( i >> 6 ) ) == ~0ul ) {
                    i += 63;
                    continue;
                }

                if ( ( *( slab->color_map + ( i >> 6 ) ) & ( 1ul << ( i % 64 ) ) ) == 0 ) {
                    *( slab->color_map + ( i >> 6 ) ) |= 1ul << ( i % 64 );
                    slab->using_count++;
                    slab->free_count--;
                    slab_cache->total_free--;
                    slab_cache->total_using++;
                    auto virtual_address = (T *)( (uint64_t)slab->virtual_address + slab_cache->size * i );
                    std::memset( (void *)virtual_address, 0, size );

                    return virtual_address;
                }
            }
        }

        return nullptr;
    }
    virtual auto deallocate( const T *address, [[maybe_unused]] size_type size = 0 ) -> void override {
        std::lock_guard guard { this->kheap_lock };
        this->_deallocate( address, size );
    }
    virtual auto _deallocate( const T *address, [[maybe_unused]] size_type size = 0 ) -> void {
        using namespace Memory::Page;
        auto page_base_address = Page::allocator< Type::P2Mib >::__page_base__( address );

        Page::allocator< Type::P2Mib > page_allocator { };
        if ( auto [ slab, slab_cache ] = this->traversal_to_find_page_base( page_base_address ); slab != nullptr && slab_cache != nullptr ) {
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
                switch ( slab_cache->size ) {
                case 32:
                case 64:
                case 128:
                case 256:
                case 512:
                    slab_cache->pool_list.remove( slab->list );
                    slab_cache->total_free -= slab->color_count;
                    page_allocator._deallocate( slab->page, 1 );
                    break;
                default:
                    slab_cache->pool_list.remove( slab->list );
                    slab_cache->total_free -= slab->color_count;
                    allocator< uint64_t > { }._deallocate( slab->color_map, slab->color_count );
                    page_allocator._deallocate( slab->page, 1 );
                    allocator< Slab > { }._deallocate( slab, slab->color_count );
                    break;
                }
            }
        }
    }
};

inline auto _kheap_allocator_nolock( std::size_t size ) -> void * {
    return allocator< char > { }._allocate( size );
}

inline auto _kheap_deallocator_nolock( const void *address, [[maybe_unused]] std::size_t size ) -> void {
    return allocator< char > { }._deallocate( (char *)address, size );
}
}     // namespace Memory::KernelHeap