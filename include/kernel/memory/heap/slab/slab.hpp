#pragma once
#include <lib/Uefi.hpp>
#include <lib/list.hpp>
PUBLIC namespace QuantumNEC::Kernel {
    struct Slab {
        Lib::ListNode list;
        VOID         *page;
        uint64_t      using_count;
        uint64_t      free_count;
        VOID         *virtual_address;
        uint64_t      color_length;
        uint64_t      color_count;
        uint64_t     *color_map;
        explicit Slab( VOID ) noexcept = default;
        ~Slab( VOID ) noexcept         = default;
    };
    struct SlabCache {
        Lib::ListTable                      pool_list;
        uint64_t                            size;
        uint64_t                            total_using;
        uint64_t                            total_free;
        Slab                               *cache_pool;
        Slab                               *cache_dma_pool;
        FuncPtr< VOID *, VOID *, uint64_t > constructor;
        FuncPtr< VOID *, VOID *, uint64_t > destructor;
        explicit SlabCache( VOID ) noexcept = default;
        ~SlabCache( VOID ) noexcept         = default;
    };
    class SlabManager {
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
        constexpr static auto   cache_size_count = sizeof( cache_size ) / sizeof( uint64_t );
        inline static SlabCache slab_caches[ cache_size_count ];

    public:
        explicit SlabManager( VOID ) noexcept;
    };
}