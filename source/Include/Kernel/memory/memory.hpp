#pragma once
#include <Kernel/memory/manager/heap/heap_manager.hpp>
#include <Kernel/memory/paging/paging.hpp>
#include <Kernel/memory/manager/page/page_manager.hpp>
#include <Kernel/memory/manager/heap/heap_manager.hpp>
#include <Kernel/memory/allocater/heap/heap_allocater.hpp>
#include <Kernel/memory/allocater/page/page_allocater.hpp>
#include <Kernel/memory/collector/heap/heap_collector.hpp>
#include <Kernel/memory/collector/page/page_collector.hpp>
#include <Kernel/memory/manager/heap/heap_manager.hpp>
#include <Kernel/memory/manager/page/page_manager.hpp>
#include <Kernel/memory/paging/paging.hpp>
#include <Kernel/memory/paging/mapper.hpp>
#include <Kernel/memory/paging/page_table.hpp>
#include <Kernel/memory/paging/ptv.hpp>
#include <Lib/Uefi.hpp>
PUBLIC namespace QuantumNEC::Kernel {
    PUBLIC class Memory :
        __page_manager,
        __paging,
        __heap_manager
    {
    public:
        explicit Memory( VOID ) noexcept;

    public:
        virtual ~Memory( VOID ) noexcept = default;
    };
}