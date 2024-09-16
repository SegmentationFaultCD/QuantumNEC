#pragma once

#include <Lib/Uefi.hpp>
#include <Lib/list.hpp>
#include <Libcxx/bitset.hpp>
#include <bit>
#include <concepts>
#include <type_traits>
#include <utility>
PUBLIC namespace QuantumNEC::Kernel {
    PUBLIC inline constexpr auto operator""_KB( IN CONST unsigned long long kib )->size_t {
        return kib * 1024;
    }

    PUBLIC inline constexpr auto operator""_MB( IN CONST unsigned long long mib )->size_t {
        return mib * 1024_KB;
    }

    PUBLIC inline constexpr auto operator""_GB( IN CONST unsigned long long gib )->size_t {
        return gib * 1024_MB;
    }
    PUBLIC enum MemoryPageType : uint64_t {
        PAGE_4K = 0b01,     // 01
        PAGE_2M = 0b10,     // 10
        PAGE_1G = 0b11,     // 11
    };
    template < auto PageSize >
    concept page_size_is_single_bit = std::is_unsigned_v< decltype( PageSize ) > && std::has_single_bit( PageSize );
    template < uint64_t PageSize >
        requires page_size_is_single_bit< PageSize >

    struct MemoryPageMaping
    {
        explicit MemoryPageMaping( VOID ) noexcept = default;
        virtual ~MemoryPageMaping( VOID ) noexcept = default;
        virtual auto allocate( IN uint64_t size ) -> VOID * = 0;
        virtual auto free( IN VOID *address, IN uint64_t size ) -> VOID = 0;

        constexpr static auto page_size = PageSize;
    };

    template < typename InfoAllocater >
        requires std::derived_from< InfoAllocater, MemoryPageMaping< InfoAllocater::page_size > >
                 && page_size_is_single_bit< InfoAllocater::page_size >
                 && requires( InfoAllocater allocater ) {
                        allocater.allocate( 0ul );
                        allocater.free( nullptr, 0ul );
                    }
    class PageHeader
    {
    public:
        constexpr static auto PAGE_INFORMATION_HEADER_SIZE { 1024 };
        constexpr static auto MEMORY_PAGE_DESCRIPTOR { ( InfoAllocater::page_size - PAGE_INFORMATION_HEADER_SIZE ) / 8 * 64 };

    public:
        enum PageState : uint64_t {
            ALL_FULL = 0,
            ALL_FREE = 1,
            NORMAL = 2
        };
        struct alignas( 1024 ) PageInformation
        {
            Lib::ListNode group_node;     // 连接每个页头
            struct PageFlags
            {
                PageState state : 2;
                MemoryPageType type : 2;
                uint64_t physical_base : 60;

                explicit PageFlags( VOID ) noexcept = default;

            } flags;     // 标志

            uint64_t map_base_adderess;     // 记录块起始地址
            uint64_t total_memory_page_count;

            std::bitset< MEMORY_PAGE_DESCRIPTOR > *bitmap_;

            explicit PageInformation( VOID ) = default;
        };
        using header_t = std::pair< PageInformation, std::bitset< MEMORY_PAGE_DESCRIPTOR > >;

    public:
        PageHeader( IN InfoAllocater &allocater, IN uint64_t _count, IN uint64_t page_type ) :
            page_header { (header_t *)allocater.allocate( count ) },
            count { _count } {
            for ( auto i = 0ul; i < count; ++i ) {
                auto &[ info, bitmap ] = this->page_header[ i ];
                info.bitmap_ = &bitmap;

                info.flags.state = PageState::ALL_FREE;
                info.flags.type = page_type;
                info.flags.physical_base = virtual_to_physical( &this->page_header[ i ] );

                info.total_memory_page_count = this->MEMORY_PAGE_DESCRIPTOR;

                info.node.container = &this->page_header[ i ];
                info.flags.page_header_group_list.push( info.group_node );
            }
        };
        template < auto PageSize >
            requires page_size_is_single_bit< PageSize >
        auto allocate( IN MemoryPageMaping< PageSize > *allocater, IN uint64_t index = 0 ) {
            PageInformation info { };
            for ( auto start { index }, end { index + count }; start < end; ++start ) {
                info = std::get< PageInformation >( page_header[ start ] );
                info.map_base_adderess = (uint64_t)allocater->allocate( this->MEMORY_PAGE_DESCRIPTOR * InfoAllocater::page_size / allocater->page_size );
            }
        }

        auto set_bit( IN uint64_t index, uint64_t size = MEMORY_PAGE_DESCRIPTOR ) -> VOID {
            return std::get< PageInformation >( this->page_header[ index ] ).bitmap_->allocate( size );
        }
        auto set_range_bit( uint64_t size = MEMORY_PAGE_DESCRIPTOR ) -> VOID {
            for ( auto i = 0ul; i < count; ++i ) {
                std::get< PageInformation >( this->page_header[ i ] ).bitmap_->allocate( size );
            }
        }
        auto get( IN uint64_t index ) const & -> const header_t & {
            return this->page_header[ index ];
        }

    private:
        uint64_t count;
        header_t *page_header;

    public:
        inline static Lib::ListTable page_header_group_list { };
    };
}