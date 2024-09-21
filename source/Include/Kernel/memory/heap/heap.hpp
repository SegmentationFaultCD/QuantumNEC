#pragma once
#include <Lib/Uefi.hpp>
#include <Lib/list.hpp>
PUBLIC namespace QuantumNEC::Kernel {
    // 内存块魔术字节
    PUBLIC constexpr CONST auto MEMORY_BLOCK_MAGIC_NUMBER { 0x1145141919810ull };
    PUBLIC class Heap
    {
    private:
        struct Zone
        {
            Lib::ListNode zone_node;
            Lib::ListTable block_table;
            uint64_t block_count;     // 记录块的数量
            VOID *zone_start_address;
            VOID *zone_end_address;
            uint64_t size;
            BOOL is_full;
            explicit Zone( VOID ) noexcept = default;
        };
        struct Block
        {
            Lib::ListNode block_node;
            Zone *owner;
            VOID *start_address;
            VOID *end_address;
            uint64_t size;
            BOOL is_free;
            uint64_t block_magic;
            explicit Block( VOID ) noexcept = default;
        };

    public:
        explicit Heap( VOID ) noexcept;

    public:
        virtual ~Heap( VOID ) noexcept = default;

    public:
        auto malloc( IN size_t size ) -> VOID *;
        auto free( IN VOID *address ) -> VOID;

    private:
        Lib::ListTable global_memory_zone_table { };
    };
}