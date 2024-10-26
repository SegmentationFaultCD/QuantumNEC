#pragma once
#include <lib/Uefi.hpp>
#include <lib/list.hpp>
PUBLIC namespace QuantumNEC::Kernel {
    struct Slab
    {
        Lib::ListNode list;
        VOID *page;
        uint64_t using_count;
        uint64_t free_count;
        VOID *virtual_address;
        uint64_t color_length;
        uint64_t color_count;
        uint64_t *color_map;
    };
    struct SlabCache
    {
        uint64_t size;
        uint64_t total_using;
        uint64_t total_free;
        Slab *cache_pool;
        Slab *cache_dma_pool;

    public:
        auto constructor( IN VOID *address, IN uint64_t arg ) -> VOID *;
        auto destructor( IN VOID *address, IN uint64_t arg ) -> VOID *;
    };
}