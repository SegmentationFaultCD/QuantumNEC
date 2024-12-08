#pragma once
#include <kernel/memory/heap/slab/slab.hpp>
#include <lib/Uefi.hpp>
#include <lib/spin_lock.hpp>
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
    inline static SlabCache slab_caches[ cache_size_count ];

public:
    enum class ErrorCode {
        CanNotFindSuitableSlabPool,
        CanNotFindSuitableSlabCache
    };

public:
    static auto traversal_to_get_slab( IN uint64_t size ) -> std::expected< SlabCache *, ErrorCode > {
        Lib::SpinLock lock;

        auto i = 0ul;
        lock.acquire( );
        for ( ; i < cache_size_count; ++i ) {
            if ( slab_caches[ i ].size >= size ) {
                lock.release( );
                return &slab_caches[ i ];
            }
        }
        lock.release( );
        return std::unexpected { ErrorCode::CanNotFindSuitableSlabCache };
    }

    static auto traversal_to_find_page_base( IN uint64_t page_base_address ) -> std::expected< std::pair< Slab *, SlabCache * >, ErrorCode > {
        Lib::SpinLock lock;
        lock.acquire( );
        for ( auto i = 0ul; i < cache_size_count; ++i ) {
            for ( auto &slab : slab_caches[ i ].pool_list ) {
                if ( (uint64_t)slab.virtual_address == page_base_address ) {
                    lock.release( );
                    return { { &slab, &slab_caches[ i ] } };
                }
            }
        }
        lock.release( );
        return std::unexpected { ErrorCode::CanNotFindSuitableSlabPool };
    }

public:
    explicit KHeapManager( void ) noexcept;
    virtual ~KHeapManager( void ) noexcept = default;
};
}     // namespace QuantumNEC::Kernel