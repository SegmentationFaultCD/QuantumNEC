#pragma once
#include <lib/Uefi.hpp>
#include <lib/list.hpp>
#include <libcxx/expected.hpp>
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
        explicit Slab( VOID ) noexcept {
        }
        ~Slab( VOID ) noexcept {
        }
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
        explicit SlabCache( VOID ) noexcept {
        }
        ~SlabCache( VOID ) noexcept {
        }
    };
}