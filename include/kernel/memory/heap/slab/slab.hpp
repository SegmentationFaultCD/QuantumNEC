#pragma once
#include <lib/Uefi.hpp>
#include <lib/list.hpp>
#include <libcxx/expected.hpp>
namespace QuantumNEC::Kernel {
struct Slab {
    Lib::ListTable< Slab >::ListNode list;
    void                            *page;
    uint64_t                         using_count;
    uint64_t                         free_count;
    void                            *virtual_address;
    uint64_t                         color_length;
    uint64_t                         color_count;
    uint64_t                        *color_map;
    explicit Slab( void ) noexcept {
    }
    ~Slab( void ) noexcept {
    }
};
struct SlabCache {
    Lib::ListTable< Slab >              pool_list;
    uint64_t                            size;
    uint64_t                            total_using;
    uint64_t                            total_free;
    Slab                               *cache_pool;
    Slab                               *cache_dma_pool;
    FuncPtr< void *, void *, uint64_t > constructor;
    FuncPtr< void *, void *, uint64_t > destructor;
    explicit SlabCache( void ) noexcept {
    }
    ~SlabCache( void ) noexcept {
    }
};
}     // namespace QuantumNEC::Kernel