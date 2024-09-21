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
    struct PageAllocaters
    {
        explicit PageAllocaters( VOID ) noexcept = default;
        virtual ~PageAllocaters( VOID ) noexcept = default;
        virtual auto allocate( IN uint64_t size ) -> VOID * = 0;
        virtual auto free( IN VOID *address, IN uint64_t size ) -> VOID = 0;

        constexpr static auto page_size = PageSize;
    };

    template < typename Allocater, typename InfoAllocater >
        requires std::derived_from< Allocater, PageAllocaters< Allocater::page_size > >
                 && std::derived_from< InfoAllocater, PageAllocaters< InfoAllocater::page_size > >
                 && page_size_is_single_bit< Allocater::page_size >
                 && page_size_is_single_bit< InfoAllocater::page_size >
                 && requires( InfoAllocater allocater ) {
                        allocater.allocate( 0ul );
                        allocater.free( nullptr, 0ul );
                    } && requires( Allocater allocater ) {
                        allocater.allocate( 0ul );
                        allocater.free( nullptr, 0ul );
                    }
    class PageHeader
    {
    public:
        constexpr static auto PAGE_INFORMATION_HEADER_SIZE { 128 };
        constexpr static auto MEMORY_PAGE_DESCRIPTOR { ( PAGE_INFORMATION_HEADER_SIZE ) / 8 * 64 };

    public:
        enum PageState : uint64_t {
            ALL_FULL = 0,
            ALL_FREE = 1,
            NORMAL = 2
        };
        struct alignas( 128 ) PageInformation
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
            uint64_t all_memory_page_count;
            uint64_t free_memory_page_count;

            std::bitset< MEMORY_PAGE_DESCRIPTOR > *bitmap_;

            explicit PageInformation( VOID ) = default;
        };
        using header_t = std::pair< PageInformation, std::bitset< MEMORY_PAGE_DESCRIPTOR > >;

    public:
        PageHeader( IN Allocater &pallocater, IN InfoAllocater &iallocater, IN uint64_t page_type ) :
            page_header { (header_t *)iallocater.allocate( 1 ) },
            type { page_type } {
            auto &[ info, bitmap ] = *this->page_header;
            info.bitmap_ = &bitmap;
            info.flags.state = PageState::ALL_FREE;
            info.flags.type = this->type;
            info.flags.physical_base = virtual_to_physical( this->page_header );
            info.all_memory_page_count = this->MEMORY_PAGE_DESCRIPTOR;
            info.free_memory_page_count = this->MEMORY_PAGE_DESCRIPTOR;
            info.group_node.container = &info;
            pallocater->page_header_group_list.append( info.group_node );
        };
        // 第一次分配专用
        PageHeader( IN Allocater &pallocater, IN VOID *address, IN uint64_t page_type, uint64_t base_address ) :
            page_header { address },
            type { page_type } {
            auto &[ info, bitmap ] = this->page_header[ 0 ];
            info.bitmap_ = &bitmap;
            info.flags.state = PageState::NORMAL;
            info.flags.type = page_type;
            info.flags.physical_base = virtual_to_physical( &this->page_header[ 0 ] );
            info.all_memory_page_count = this->MEMORY_PAGE_DESCRIPTOR;
            info.free_memory_page_count = this->MEMORY_PAGE_DESCRIPTOR;
            info.group_node.container = &info;
            info.map_base_adderess = base_address;
            pallocater->page_header_group_list.append( info.group_node );
        }
        template < typename AdvancedAllocater >
            requires( !std::is_same_v< AdvancedAllocater, Allocater > )
        auto allocate( IN AdvancedAllocater &allocater ) {
            // 拿来取得mapbase的
            // 比如我现在是2M分配器， 有512个根
            // 那么则需要1g分配器分配一个1g来给这512个2m
            // AdvancedAllocater 便是这个1g分配器
            std::get< PageInformation >( *this->page_header ).map_base_adderess = (uint64_t)allocater.allocate( this->MEMORY_PAGE_DESCRIPTOR * Allocater::page_size / allocater.page_size );
        }

        auto set_range_bit( uint64_t size = MEMORY_PAGE_DESCRIPTOR ) -> VOID {
            std::get< PageInformation >( *this->page_header ).bitmap_->allocate( size );
        }
        auto get( VOID ) const & -> header_t & {
            return this->page_header[ 0 ];
        }

    private:
        header_t *page_header;
        uint64_t type;
    };
}