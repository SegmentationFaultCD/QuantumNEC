#pragma once
#include <Lib/Types/Uefi.hpp>
#include <Lib/Types/type_bool.hpp>
#include <Lib/STL/list>
PUBLIC namespace QuantumNEC::Kernel {
    // 内存块魔术字节
    PUBLIC constexpr CONST auto MEMORY_BLOCK_MAGIC_NUMBER { 0x1145141919810ull };
    PUBLIC class HeapMemory
    {
    private:
        struct Zone
        {
            Lib::STL::ListNode zone_node;
            Lib::STL::ListTable block_table;
            Lib::Types::uint64_t block_count;     // 记录块的数量
            Lib::Types::Ptr< VOID > zone_start_address;
            Lib::Types::Ptr< VOID > zone_end_address;
            Lib::Types::uint64_t size;
            Lib::Types::BOOL is_full;
        };
        struct Block
        {
            Lib::STL::ListNode block_node;
            Lib::Types::Ptr< Zone > owner;
            Lib::Types::Ptr< VOID > start_address;
            Lib::Types::Ptr< VOID > end_address;
            Lib::Types::uint64_t size;
            Lib::Types::BOOL is_free;
            Lib::Types::uint64_t block_magic;
        };

    public:
        explicit HeapMemory( IN Lib::Types::Ptr< Lib::Types::BootConfig > _config ) noexcept;

    public:
        virtual ~HeapMemory( VOID ) noexcept = default;

    public:
        auto malloc( IN Lib::Types::size_t size ) -> Lib::Types::Ptr< VOID >;
        auto free( IN Lib::Types::Ptr< VOID > address ) -> VOID;

    private:
        inline STATIC Lib::STL::ListTable global_memory_zone_table { };
    };
}