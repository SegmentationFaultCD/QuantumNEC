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
        constexpr static auto PAGE_BITMAP_SIZE { 128 };
        constexpr static auto MEMORY_PAGE_DESCRIPTOR { PAGE_INFORMATION_HEADER_SIZE / 8 * 64 };
        constexpr static auto MEMORY_PAGE_HEADER_COUNT { InfoAllocater::page_size / ( PAGE_INFORMATION_HEADER_SIZE + PAGE_BITMAP_SIZE ) };
        // 一个PageHeader所指向的内存根数量便是MEMORY_PAGE_DESCRIPTOR*MEMORY_PAGE_HEADER_COUNT
    public:
        enum PageState : uint64_t {
            ALL_FULL = 0,
            ALL_FREE = 1,
            NORMAL = 2
        };
        struct alignas( PAGE_INFORMATION_HEADER_SIZE ) PageInformation
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

            PageInformation *next;
            PageInformation *prev;

            explicit PageInformation( VOID ) = default;
        };
        using header_t = std::pair< PageInformation, std::bitset< MEMORY_PAGE_DESCRIPTOR > >;

    public:
        // 这里group_count 必须是有多少个iallocater分配的块
        PageHeader( IN Allocater &pallocater, IN InfoAllocater &iallocater, IN uint64_t page_type, IN uint64_t group_count ) :
            page_header { (header_t *)iallocater.allocate( group_count ) },
            type { page_type },
            group_count { group_count } {
            if ( group_count ) {
                auto &[ head_info, head_bitmap ] = this->page_header[ 0 ];
                pallocater->page_header_group_list.append( head_info.group_node );
                head_info.group_node.container = &head_info;
                head_info.flags.state = PageState::ALL_FREE;
                head_info.flags.type = this->type;
                head_info.flags.physical_base = virtual_to_physical( this->page_header );
                head_info.all_memory_page_count = this->MEMORY_PAGE_DESCRIPTOR;
                head_info.free_memory_page_count = this->MEMORY_PAGE_DESCRIPTOR;
                head_info.bitmap_ = &head_bitmap;
                head_info.next = nullptr;
                head_info.prev = reinterpret_cast< header_t * >( &head_info ) - 1;
                for ( auto j = 1ul; j < ( MEMORY_PAGE_HEADER_COUNT * this->group_count - 1 ); ++j ) {
                    auto &[ mid_info, mid_bitmap ] = this->page_header[ j ];
                    mid_info.flags.state = PageState::ALL_FREE;
                    mid_info.flags.type = this->type;
                    mid_info.flags.physical_base = virtual_to_physical( this->page_header );
                    mid_info.all_memory_page_count = this->MEMORY_PAGE_DESCRIPTOR;
                    mid_info.free_memory_page_count = this->MEMORY_PAGE_DESCRIPTOR;
                    mid_info.bitmap_ = &mid_bitmap;
                    mid_info.next = reinterpret_cast< header_t * >( &mid_info ) + 1;
                    mid_info.prev = reinterpret_cast< header_t * >( &mid_info ) - 1;
                }
                auto &[ end_info, end_bitmap ] = this->page_header[ MEMORY_PAGE_HEADER_COUNT * this->group_count - 1 ];
                end_info.flags.state = PageState::ALL_FREE;
                end_info.flags.type = this->type;
                end_info.flags.physical_base = virtual_to_physical( this->page_header );
                end_info.all_memory_page_count = this->MEMORY_PAGE_DESCRIPTOR;
                end_info.free_memory_page_count = this->MEMORY_PAGE_DESCRIPTOR;
                end_info.bitmap_ = &end_bitmap;
                end_info.next = nullptr;
                end_info.prev = reinterpret_cast< header_t * >( &end_info ) - 1;
            }
        };
        PageHeader( IN PageInformation *phis ) {
            this->page_header = phis;
            this->type = phis->flags.type;
            for ( this->group_count = 1; phis; phis = phis->next ) {
                this->group_count++;
            }
            // 头数
            auto header_count = !this->group_count % MEMORY_PAGE_DESCRIPTOR ? this->group_count / MEMORY_PAGE_DESCRIPTOR : DIV_ROUND_UP( this->group_count, MEMORY_PAGE_DESCRIPTOR );
            // 组数
            this->group_count = header_count % MEMORY_PAGE_HEADER_COUNT ? header_count / MEMORY_PAGE_HEADER_COUNT + 1 : header_count / MEMORY_PAGE_HEADER_COUNT;
        };
        // 第一次分配专用
        PageHeader( IN Allocater &pallocater, IN VOID *address, IN uint64_t page_type ) :
            page_header { address },
            type { page_type },
            group_count { 1 } {
            if ( group_count ) {
                auto &[ head_info, head_bitmap ] = this->page_header[ 0 ];
                pallocater->page_header_group_list.append( head_info.group_node );
                head_info.group_node.container = &head_info;
                head_info.flags.state = PageState::ALL_FREE;
                head_info.flags.type = this->type;
                head_info.flags.physical_base = virtual_to_physical( this->page_header );
                head_info.all_memory_page_count = this->MEMORY_PAGE_DESCRIPTOR;
                head_info.free_memory_page_count = this->MEMORY_PAGE_DESCRIPTOR;
                head_info.bitmap_ = &head_bitmap;
                head_info.next = nullptr;
                head_info.prev = reinterpret_cast< header_t * >( &head_info ) - 1;
                for ( auto j = 1ul; j < MEMORY_PAGE_HEADER_COUNT - 1; ++j ) {
                    auto &[ mid_info, mid_bitmap ] = this->page_header[ j ];
                    mid_info.flags.state = PageState::ALL_FREE;
                    mid_info.flags.type = this->type;
                    mid_info.flags.physical_base = virtual_to_physical( this->page_header );
                    mid_info.all_memory_page_count = this->MEMORY_PAGE_DESCRIPTOR;
                    mid_info.free_memory_page_count = this->MEMORY_PAGE_DESCRIPTOR;
                    mid_info.bitmap_ = &mid_bitmap;
                    mid_info.next = reinterpret_cast< header_t * >( &mid_info ) + 1;
                    mid_info.prev = reinterpret_cast< header_t * >( &mid_info ) - 1;
                }
                auto &[ end_info, end_bitmap ] = this->page_header[ MEMORY_PAGE_HEADER_COUNT - 1 ];
                end_info.flags.state = PageState::ALL_FREE;
                end_info.flags.type = this->type;
                end_info.flags.physical_base = virtual_to_physical( this->page_header );
                end_info.all_memory_page_count = this->MEMORY_PAGE_DESCRIPTOR;
                end_info.free_memory_page_count = this->MEMORY_PAGE_DESCRIPTOR;
                end_info.bitmap_ = &end_bitmap;
                end_info.next = nullptr;
                end_info.prev = reinterpret_cast< header_t * >( &end_info ) - 1;
            }
        };
        template < typename AdvancedAllocater >
            requires( !std::is_same_v< AdvancedAllocater, Allocater > )
        auto allocate( IN AdvancedAllocater &allocater, IN uint64_t size ) {
            auto base_map_address = (uint64_t)allocater.allocate( ( this->MEMORY_PAGE_DESCRIPTOR * Allocater::page_size / allocater.page_size ) * this->group_count * this->MEMORY_PAGE_HEADER_COUNT );
            this->allocate( size, base_map_address );
        }

        auto allocate( IN uint64_t size, IN uint64_t base_map_address ) {
            // 拿来取得mapbase的
            // 比如我现在是2M分配器， 有512个根
            // 那么则需要1g分配器分配一个1g来给这512个2m
            // AdvancedAllocater 便是这个1g分配器
            if ( this->group_count ) {
                // 设置每个base_map_address
                // 一共有this->group_count*this->MEMORY_PAGE_HEADER_COUNT个base_map_address要设置

                for ( auto i = 0ul; i < this->group_count * this->MEMORY_PAGE_HEADER_COUNT; ++i ) {
                    std::get< PageInformation >( this->page_header[ i ] ).map_base_adderess = base_map_address + ( this->MEMORY_PAGE_DESCRIPTOR * Allocater::page_size ) * i;
                    // 取得base_map_address的计算公式为 分配起始地址 + 内存描述符数量 * 内存页大小 * 所处页头index（这里的计算公式为组index * 内存描述符数量 + 所处组里的index）
                }
                auto remainder = size % this->MEMORY_PAGE_DESCRIPTOR;
                auto end = remainder ? size / this->MEMORY_PAGE_DESCRIPTOR + 1 : size / this->MEMORY_PAGE_DESCRIPTOR;
                for ( auto i = 0ul; i < end - 1; ++i ) {
                    auto &info = std::get< PageInformation >( this->page_header[ i ] );
                    info.flags.state = PageState::ALL_FULL;
                    info.free_memory_page_count = 0;
                    info.bitmap_->set( 0, this->MEMORY_PAGE_DESCRIPTOR );
                }
                auto &info = std::get< PageInformation >( this->page_header[ end - 1 ] );
                info.flags.state = PageState::NORMAL;
                info.free_memory_page_count -= remainder;
                info.bitmap_->set( 0, remainder );
            }
        }

        auto set_range_bit( IN uint64_t index, uint64_t size = MEMORY_PAGE_DESCRIPTOR ) -> VOID {
            std::get< PageInformation >( this->page_header[ index ] ).bitmap_->allocate( size );
        }
        auto set_range_bit( VOID ) -> VOID {
            for ( auto i = 0ul; i < this->count * this->page_header; ++i )
                for ( auto j = 0ul; j < this->MEMORY_PAGE_HEADER_COUNT; ++j )
                    std::get< PageInformation >( this->page_header[ i * this->MEMORY_PAGE_HEADER_COUNT + j ] ).bitmap_->allocate( this->MEMORY_PAGE_DESCRIPTOR );
        }
        auto get( IN uint64_t index ) const & -> header_t & {
            return this->page_header[ index ];
        }

    private:
        header_t *page_header;
        uint64_t type;
        uint64_t group_count;
    };
}