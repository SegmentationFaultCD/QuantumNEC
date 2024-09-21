#pragma once
#include <Kernel/memory/heap/heap.hpp>
#include <Kernel/memory/paging_map/paging_map.hpp>
#include <Kernel/memory/page_allocater/page_allocater.hpp>
#include <Lib/Uefi.hpp>
PUBLIC namespace QuantumNEC::Kernel {
    PUBLIC class Memory
    {
    public:
        explicit Memory( VOID ) noexcept;

    public:
        virtual ~Memory( VOID ) noexcept = default;

        inline STATIC Heap *heap;
        inline STATIC PageAllocater *page;
        inline STATIC PagingMap *memory_paging_map;
    };
}