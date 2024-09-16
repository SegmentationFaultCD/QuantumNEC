#pragma once
#include <Kernel/memory/heap.hpp>
#include <Kernel/memory/paging_map.hpp>
#include <Kernel/memory/page.hpp>
#include <Lib/Uefi.hpp>
PUBLIC namespace QuantumNEC::Kernel {
    PUBLIC class Memory
    {
    public:
        explicit Memory( VOID ) noexcept;

    public:
        virtual ~Memory( VOID ) noexcept = default;

        inline STATIC Heap *heap;
        inline STATIC Page *page;
        inline STATIC PagingMap *memory_paging_map;
    };
}