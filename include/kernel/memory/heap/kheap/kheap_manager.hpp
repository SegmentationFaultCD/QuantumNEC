#pragma once
#include <kernel/memory/heap/slab/slab.hpp>
#include <lib/Uefi.hpp>
#include <utility>
PUBLIC namespace QuantumNEC::Kernel {
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
        STATIC auto traversal_to_get_slab( IN uint64_t size ) -> std::expected< SlabCache *, ErrorCode > {
            auto i = 0ul;
            for ( ; i < cache_size_count; ++i ) {
                if ( slab_caches[ i ].size >= size ) {
                    return &slab_caches[ i ];
                }
            }
            return std::unexpected { ErrorCode::CanNotFindSuitableSlabCache };
        }

        STATIC auto traversal_to_find_page_base( IN uint64_t page_base_address ) -> std::expected< std::pair< Slab *, SlabCache * >, ErrorCode > {
            for ( auto i = 0ul; i < cache_size_count; ++i ) {
                if ( auto slab = slab_caches[ i ].pool_list.traversal(
                         []( Lib::ListNode *node, uint64_t vaddress ) static -> BOOL {
                             return (uint64_t)( (Slab *)node )->virtual_address == vaddress;
                         },
                         page_base_address );
                     slab ) {
                    return { { (Slab *)slab->container, &slab_caches[ i ] } };
                }
            }
            return std::unexpected { ErrorCode::CanNotFindSuitableSlabPool };
        }

    public:
        explicit KHeapManager( VOID ) noexcept;
        virtual ~KHeapManager( VOID ) noexcept = default;
    };
}