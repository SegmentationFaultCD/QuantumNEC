#pragma once
#include <kernel/memory/heap/slab/slab.hpp>
#include <lib/Uefi.hpp>
#include <lib/smart_spinlock.hpp>
#include <utility>
namespace QuantumNEC::Kernel {
class KHeapManager {
private:
    constexpr static uint64_t cache_size[] {
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

private:
    inline static Lib::smart_spinlock< SlabCache > slab_caches[ cache_size_count ] { };

public:
    enum class ErrorCode {
        CanNotFindSuitableSlabPool,
        CanNotFindSuitableSlabCache
    };

public:
    static auto traversal_to_get_slab( IN uint64_t size ) -> std::expected< Lib::smart_spinlock< SlabCache > *, ErrorCode > {
        auto i = 0ul;
        for ( ; i < cache_size_count; ++i ) {
            auto result = slab_caches[ i ].visit( [ & ]( SlabCache &slab_cache ) -> Lib::smart_spinlock< SlabCache > * {
                if ( slab_cache.size >= size ) {
                    return &slab_caches[ i ];
                }
                return NULL;
            } );
            if ( result ) {
                return result;
            }
        }
        return std::unexpected { ErrorCode::CanNotFindSuitableSlabCache };
    }

    static auto traversal_to_find_page_base( IN uint64_t page_base_address ) -> std::expected< std::pair< Slab *, Lib::smart_spinlock< SlabCache > * >, ErrorCode > {
        for ( auto i = 0ul; i < cache_size_count; ++i ) {
            auto result = slab_caches[ i ].visit( [ &page_base_address ]( SlabCache &slab_cache ) -> Slab * {
                for ( auto &slab : slab_cache.pool_list ) {
                    if ( (uint64_t)slab.virtual_address == page_base_address ) {
                        return &slab;
                    }
                }
                return NULL;
            } );
            if ( result ) {
                return { { result, &slab_caches[ i ] } };
            }
        }
        return std::unexpected { ErrorCode::CanNotFindSuitableSlabPool };
    }

public:
    explicit KHeapManager( void ) noexcept;
    ~KHeapManager( void ) noexcept = default;
};
}     // namespace QuantumNEC::Kernel