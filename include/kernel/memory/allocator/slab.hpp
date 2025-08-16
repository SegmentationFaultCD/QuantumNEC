#pragma once
#include <lib/list.hpp>
namespace Memory {

struct Slab {
    Library::List< Slab >::ListNode list;
    void                           *page;
    uint64_t                        using_count;
    uint64_t                        free_count;
    void                           *virtual_address;
    uint64_t                        color_length;
    uint64_t                        color_count;
    uint64_t                       *color_map;
};
struct SlabCache {
    Library::List< Slab > pool_list;
    uint64_t              size;
    uint64_t              total_using;
    uint64_t              total_free;
    Slab                 *cache_pool;
    Slab                 *cache_dma_pool;
    void *( *constructor )( void *, uint64_t );
    void *( *destructor )( void *, uint64_t );
};
}     // namespace Memory