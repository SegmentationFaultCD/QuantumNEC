#pragma once
#include <Lib/Uefi.hpp>
#include <Lib/list.hpp>
#include <Kernel/memory/allocater/heap/heap_allocater.hpp>
#include <Kernel/memory/collector/heap/heap_collector.hpp>
PUBLIC namespace QuantumNEC::Kernel {
    PUBLIC class __heap_manager
    {
        friend __heap_allocater;
        friend __heap_collector;

    public:
        struct __zone
        {
            Lib::ListNode zone_node;
            Lib::ListTable block_table;
            uint64_t block_count;     // 记录块的数量
            VOID *zone_start_address;
            VOID *zone_end_address;
            uint64_t size;
            BOOL is_full;
            explicit __zone( VOID ) noexcept = default;
        };
        struct __block
        {
            Lib::ListNode block_node;
            __zone *owner;
            VOID *start_address;
            VOID *end_address;
            uint64_t size;
            BOOL is_free;
            uint64_t block_magic;
            explicit __block( VOID ) noexcept = default;
        };
        constexpr static auto memory_block_magic_size { 0x1145141919810ull };

    public:
        explicit __heap_manager( VOID ) noexcept;
        virtual ~__heap_manager( VOID ) noexcept = default;

    private:
        inline STATIC Lib::ListTable global_memory_zone_table;
    };
}