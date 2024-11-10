#pragma once
#include <concepts>
#include <kernel/memory/arch/memory_arch.hpp>
#include <kernel/memory/heap/kheap/kheap_allocater.hpp>
#include <kernel/memory/page/page_allocater.hpp>
#include <kernel/memory/page/page_manager.hpp>
#include <kernel/print.hpp>
#include <lib/Uefi.hpp>
#include <utility>
PUBLIC namespace QuantumNEC::Kernel {
    template < MemoryPageType __allocater_to_bind__, MemoryPageType __mmap_allocater__ >
        requires( __allocater_to_bind__ != NONE )
    PUBLIC class __page_header__ {
    public:
        constexpr static auto page_information_block_size { 128 };
        constexpr static auto page_bitmap_size { 128 };
        constexpr static auto page_descriptor_count { page_bitmap_size << 3 };     // 一个bitmap所有bit
        constexpr static auto page_header_count_of_zone = ( [] consteval static {
            using enum MemoryPageType;
            ;
            switch ( __allocater_to_bind__ ) {
            case PAGE_1G:
                return 1024ul;     // 可以管1048576gi b内存
            case PAGE_2M:
                return 128ul;     // 可以管理16gi b内存
            case PAGE_4K:
                return 4ul;
            }
        } )( );
        struct alignas( page_information_block_size ) __page_information__ {
            Lib::ListNode group_node;     // 连接每个页头
            struct __page_flags__ {
                enum class __page_state__ : uint64_t {
                    ALL_FULL = 0,
                    ALL_FREE = 1,
                    NORMAL   = 2
                };
                __page_state__ state : 2;
                uint64_t       type : 2;
                uint64_t : 60;

                explicit __page_flags__( VOID ) noexcept = default;

            } flags;                                                // 标志
            uint64_t                              base_address;     // 记录块起始地址
            uint64_t                              free_memory_page_count;
            std::bitset< page_descriptor_count > *bitmap;
            __page_information__                 *owner;
            uint64_t                              header_count;
            explicit __page_information__( VOID ) = default;
        };
        using header_t = std::pair< __page_information__, std::bitset< page_descriptor_count > >;

        constexpr static auto header_size { sizeof( header_t ) };

        uint64_t all_memory_header_count;
        uint64_t all_memory_page_desvriptor_count;

    private:
        class __address__ : PageAllocater, KHeapAllocater {
        public:
            enum __address_type__ {
                BASE_ADDRESS,
                HEADER_START_ADDRESS
            };

        public:
            __address__( VOID ) :
                has_val { FALSE },
                address { 0 } {
            }
            __address__( uint64_t address ) :
                has_val { TRUE },
                address { address } {
            }
            template < __address_type__ __type__ >
            auto get_address( IN uint64_t header_count ) {
                if ( has_val ) {
                    if constexpr ( __type__ == HEADER_START_ADDRESS ) {
                        return reinterpret_cast< header_t * >( address );
                    }
                    else {
                        return address;
                    }
                }
                if constexpr ( __type__ == HEADER_START_ADDRESS ) {
                    return reinterpret_cast< header_t * >( this->KHeapAllocater::allocate( header_count * header_size ) );
                }
                else {
                    if constexpr ( __mmap_allocater__ != NONE ) {
                        return (uint64_t)this->PageAllocater::allocate< __mmap_allocater__ >(
                            (( this->__page_size__< __allocater_to_bind__ > * header_count * page_descriptor_count ) / this->__page_size__< __mmap_allocater__ >));
                    }
                    return 0ul;
                }
            }

        private:
            BOOL     has_val;
            uint64_t address;
        };

    private:
        using __page_state__ = __page_information__::__page_flags__::__page_state__;

    public:
        explicit __page_header__( IN uint64_t header_count, IN __address__ header_start_address, IN __address__ base_address ) {
            auto &table = allocate_information_list[ __allocater_to_bind__ ];

            this->all_memory_header_count          = Lib::DIV_ROUND_UP( header_count, this->page_header_count_of_zone );
            this->all_memory_page_desvriptor_count = all_memory_header_count * page_descriptor_count;

            this->group = header_start_address.template get_address< __address__::HEADER_START_ADDRESS >( this->all_memory_header_count );

            auto base_address_ = base_address.template get_address< __address__::BASE_ADDRESS >( this->all_memory_header_count );

            this->group[ 0 ].first.header_count = this->all_memory_header_count;
            for ( auto i = 0ul; i < this->all_memory_header_count; i += this->page_header_count_of_zone ) {
                auto &[ info, bitmap ] = this->group[ i ];
                table.append( info.group_node );
                info.group_node.container = &info;
            }

            for ( auto i = 0ul; i < this->all_memory_header_count; ++i ) {
                auto &[ info, bitmap ]      = this->group[ i ];
                info.owner                  = &this->group[ 0 ].first;
                info.flags.state            = __page_state__::ALL_FREE;
                info.flags.type             = __allocater_to_bind__;
                info.free_memory_page_count = this->page_descriptor_count;
                info.bitmap                 = &bitmap;
                info.base_address           = base_address_ + this->page_descriptor_count * PageAllocater::__page_size__< __allocater_to_bind__ > * i;
            }
        }
        explicit __page_header__( IN __page_information__ *phis ) {
            this->group = reinterpret_cast< header_t * >( phis->owner );

            this->all_memory_header_count          = phis->owner->header_count;
            this->all_memory_page_desvriptor_count = phis->owner->header_count * page_descriptor_count;
        }
        auto __allocate_headers__( IN uint64_t __size__ ) {
            auto &&remainder = __size__ % this->page_descriptor_count;
            auto &&end       = Lib::DIV_ROUND_UP( __size__, this->page_descriptor_count );

            uint64_t i { };
            while ( i < this->all_memory_header_count ) {
                auto &header = std::get< __page_information__ >( this->group[ i ] );
                if ( header.flags.state != __page_state__::ALL_FREE ) {
                    for ( auto j = i; j < i + end; ++j ) {
                        if ( std::get< __page_information__ >( group[ j ] ).flags.state != __page_state__::ALL_FREE ) {
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
                auto &info                  = std::get< __page_information__ >( this->group[ i ] );
                info.flags.state            = __page_state__::ALL_FULL;
                info.free_memory_page_count = 0;
                info.bitmap->set( 0, this->page_descriptor_count );
            }
            auto &end_info       = std::get< __page_information__ >( this->group[ end - 1 ] );
            end_info.flags.state = __page_state__::NORMAL;
            end_info.free_memory_page_count -= remainder;
            end_info.bitmap->set( 0, remainder );
        }
        auto get( IN uint64_t index ) const & -> header_t & {
            return this->group[ index ];
        }

    private:
        header_t *group;
    };
}