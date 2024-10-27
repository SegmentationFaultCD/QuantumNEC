#pragma once
#include <lib/Uefi.hpp>
#include <kernel/memory/page/page_manager.hpp>
#include <kernel/memory/page/page_allocater.hpp>
#include <kernel/memory/arch/memory_arch.hpp>
#include <concepts>
#include <utility>
#include <kernel/print.hpp>
PUBLIC namespace QuantumNEC::Kernel {
    template < MemoryPageType __allocater_to_bind, MemoryPageType _prev_info_block_allocater, MemoryPageType _mmap_allocater >
    PUBLIC class __page_header
    {
    public:
        constexpr static auto page_information_block_size { 128 };
        constexpr static auto page_bitmap_size { 128 };
        constexpr static auto page_descriptor_count { page_bitmap_size << 3 };     // 一个bitmap所有bit
        constexpr static auto page_header_count { PageAllocater::__page_size< _prev_info_block_allocater > / ( page_information_block_size + page_bitmap_size ) };

        enum __page_state : uint64_t {
            ALL_FULL = 0,
            ALL_FREE = 1,
            NORMAL = 2
        };
        struct alignas( page_information_block_size ) __page_information
        {
            Lib::ListNode group_node;     // 连接每个页头

            struct __page_flags
            {
                __page_state state : 2;
                uint64_t type : 2;
                uint64_t reserved : 60;

                explicit __page_flags( VOID ) noexcept = default;

            } flags;     // 标志

            uint64_t base_adderess;     // 记录块起始地址

            uint64_t free_memory_page_count;

            std::bitset< page_descriptor_count > *bitmap;

            __page_information *owner;
            uint64_t header_count;

            explicit __page_information( VOID ) = default;
        };
        using header_t = std::pair< __page_information, std::bitset< page_descriptor_count > >;
        uint64_t all_memory_header_count;
        uint64_t all_memory_page_desvriptor_count;

    public:
        explicit __page_header( IN uint64_t group_count, IN std::pair< uint64_t, int64_t > __header_start_address, IN std::pair< uint64_t, int64_t > __base_address ) {
            auto &table = allocate_information_list[ __allocater_to_bind ];

            if ( std::get< 1 >( __header_start_address ) == -1 ) {
                this->group = reinterpret_cast< header_t * >( physical_to_virtual( allocater.allocate< _prev_info_block_allocater >( group_count ) ) );
            }
            else {
                this->group = reinterpret_cast< header_t * >( std::get< 0 >( __header_start_address ) );
            }
            this->all_memory_header_count = this->page_header_count * group_count;
            this->all_memory_page_desvriptor_count = all_memory_header_count * this->page_descriptor_count;

            auto &[ start_info, start_bitmap ] = this->group[ 0 ];

            table.append( start_info.group_node );

            start_info.header_count = this->all_memory_header_count;
            start_info.owner = &start_info;
            start_info.group_node.container = &start_info;
            start_info.flags.state = __page_state::ALL_FREE;
            start_info.flags.type = __allocater_to_bind;
            start_info.free_memory_page_count = this->page_descriptor_count;
            start_info.bitmap = &start_bitmap;

            for ( auto i = 0ul; i < this->all_memory_header_count / page_header_count; ++i ) {
                auto info = std::get< __page_information >( this->group[ i ] );
                table.append( info.group_node );
                info.group_node.container = &info;
            }

            for ( auto j = 1ul; j < this->all_memory_header_count; ++j ) {
                auto &[ end_info, end_bitmap ] = this->group[ j ];
                end_info.header_count = this->all_memory_header_count;
                end_info.owner = &start_info;
                end_info.flags.state = __page_state::ALL_FREE;
                end_info.flags.type = __allocater_to_bind;
                end_info.free_memory_page_count = this->page_descriptor_count;
                end_info.bitmap = &end_bitmap;
            }
            auto base_address = std::get< 0 >( __base_address );

            if ( std::get< 1 >( __base_address ) == -1 ) {
                if constexpr ( _mmap_allocater != MemoryPageType::NONE ) {
                    base_address = (uint64_t)allocater.allocate< _mmap_allocater >( (( PageAllocater::__page_size< __allocater_to_bind > * this->all_memory_page_desvriptor_count ) / PageAllocater::__page_size< _mmap_allocater >));
                }
            }

            for ( auto i = 0ul; i < this->all_memory_header_count; ++i ) {     // 对每个块的base_adderess进行初始化
                std::get< __page_information >( this->group[ i ] ).base_adderess = base_address + this->page_descriptor_count * PageAllocater::__page_size< __allocater_to_bind > * i;
            }
        }
        explicit __page_header( IN __page_information *phis ) {
            this->group = reinterpret_cast< header_t * >( phis->owner );
            this->all_memory_header_count = phis->owner->header_count;
            this->all_memory_page_desvriptor_count = phis->owner->header_count * page_descriptor_count;
        }
        auto __allocate_headers( IN uint64_t __size ) {
            auto &&remainder = __size % this->page_descriptor_count;
            auto &&end = remainder ? __size / this->page_descriptor_count + 1 : __size / this->page_descriptor_count;
            uint64_t i { };
            while ( i < this->all_memory_header_count ) {
                auto &header = std::get< __page_information >( this->group[ i ] );
                if ( header.flags.type != __page_state::ALL_FREE ) {
                    for ( auto j = i; j < i + end; ++j ) {
                        if ( std::get< __page_information >( group[ j ] ).flags.type != __page_state::ALL_FREE ) {
                            goto out;
                        }
                    }
                    end += i;
                    break;
                }
            out:
                ++i;
            }
            for ( ; i < end - 1; ++i ) {
                // 把开头到结尾前一个都算进去
                auto &info = std::get< __page_information >( this->group[ i ] );
                info.flags.state = __page_state::ALL_FULL;
                info.free_memory_page_count = 0;
                info.bitmap->set( 0, this->page_descriptor_count );
            }
            auto &end_info = std::get< __page_information >( this->group[ end - 1 ] );
            end_info.flags.state = __page_state::NORMAL;
            end_info.free_memory_page_count -= remainder;
            end_info.bitmap->set( 0, remainder );
        }
        auto get( IN uint64_t index ) const & -> header_t & {
            return this->group[ index ];
        }

    private:
        header_t *group;
        PageAllocater allocater { };
    };
}