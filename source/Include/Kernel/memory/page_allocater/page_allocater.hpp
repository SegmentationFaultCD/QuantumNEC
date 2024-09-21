#pragma once
#include <Kernel/memory/paging_map/paging_map.hpp>

#include <Kernel/memory/page_allocater/page_allocater_1G.hpp>
#include <Kernel/memory/page_allocater/page_allocater_4K.hpp>
#include <Kernel/memory/page_allocater/page_allocater_2M.hpp>
PUBLIC namespace QuantumNEC::Kernel {
    PUBLIC class PageAllocater
    {
        friend PagingMap;

    public:
        explicit PageAllocater( VOID ) noexcept;

    public:
        virtual ~PageAllocater( VOID ) noexcept = default;

    public:
        auto allocate( IN size_t size, IN MemoryPageType type ) -> VOID *;
        auto free( IN VOID *address, IN size_t size, IN MemoryPageType type ) -> VOID;

    public:
        size_t all_memory_total { };
        size_t free_memory_total { };

    private:
        PageAllocater2M p2m { };
        PageAllocater1G p1g { };
        PageAllocater4K p4k { };
    };
}