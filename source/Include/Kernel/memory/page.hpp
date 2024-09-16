#pragma once
#include <Kernel/memory/paging_map.hpp>

#include <Kernel/memory/memory_page_map/memory_map_1G.hpp>
#include <Kernel/memory/memory_page_map/memory_map_4K.hpp>
#include <Kernel/memory/memory_page_map/memory_map_2M.hpp>
PUBLIC namespace QuantumNEC::Kernel {
    PUBLIC class Page
    {
        friend PagingMap;

    public:
        explicit Page( VOID ) noexcept;

    public:
        virtual ~Page( VOID ) noexcept = default;

    public:
        auto malloc( IN size_t size, IN MemoryPageType type ) -> VOID *;
        auto free( IN VOID *address, IN size_t size, IN MemoryPageType type ) -> VOID;

    public:
        size_t all_memory_total { };
        size_t free_memory_total { };

    private:
        MemoryPageMap2M mpg2m { };
        MemoryPageMap1G mpg1g { };
        MemoryPageMap4K mpg4k { };
    };
}