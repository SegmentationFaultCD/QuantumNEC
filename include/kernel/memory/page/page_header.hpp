#pragma once
#include <concepts>
#include <kernel/memory/arch/memory_arch.hpp>
#include <kernel/memory/heap/kheap/kheap_walker.hpp>
#include <kernel/memory/page/page_manager.hpp>
#include <kernel/memory/page/page_walker.hpp>
#include <kernel/print.hpp>
#include <lib/Uefi.hpp>
#include <lib/rbtree.hpp>
#include <lib/spin_lock.hpp>
#include <utility>
PUBLIC namespace QuantumNEC::Kernel {
    template < MemoryPageType __allocater_to_bind__, MemoryPageType __mmap_allocater__ >
        requires( __allocater_to_bind__ != NONE )
    PUBLIC class __page_header__ {
    public:
        class __helper__ {
            constexpr static auto page_information_block_size { 128 };
            constexpr static auto page_bitmap_size { 128 };

        public:
            constexpr static auto page_descriptor_count { page_bitmap_size << 3 };     // 一个bitmap所有bit

            struct __page_information__;

            using __node_key__   = uint64_t;
            using __group_type__ = RBTree< __page_information__, __node_key__ >;
            using __header__     = std::pair< __page_information__, std::bitset< __helper__::page_descriptor_count > >;

            struct _aligned( page_information_block_size ) __page_information__ {
                __group_type__::RBTreeNode group_node;     // 连接每个页头
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

                } flags;                                                            // 标志
                uint64_t                                          base_address;     // 记录块起始地址,这个也是Key
                uint64_t                                          free_memory_page_count;
                std::bitset< __helper__::page_descriptor_count > *bitmap;
                uint64_t                                          header_count;     // 一个区域所包含的头数
                __page_information__                             *owner;
                // 其中
                // (Zone‘s memory total)normal = header_count * page_descriptor_count * page size
                // (Zone‘s memory total)min = page_descriptor_count * page size, 也就是只有一个页

                explicit __page_information__( VOID ) = default;
            };

            constexpr static auto header_size { sizeof( __header__ ) };

        public:
            constexpr static auto __zone_min_memory__ = PageWalker::__page_size__< __allocater_to_bind__ > * page_descriptor_count;

            constexpr static auto __zone_memory_mask_low__( IN uint64_t header_count ) {
                return ( __zone_min_memory__ * header_count - 1 );
            };
            consteval static auto __zone_memory_mask_low__( ) {
                return ( __zone_min_memory__ - 1 );
            };
            constexpr static auto __zone_memory_mask__( IN uint64_t header_count ) {
                return ~__zone_memory_mask_low__( header_count );
            };
            consteval static auto __zone_memory_mask__( ) {     // min
                return ~__zone_memory_mask_low__( );
            };

        public:
            static auto get_group( VOID ) -> __group_type__ & {
                return page_header_group[ __allocater_to_bind__ ];
            }
            static auto get_keys( IN auto base_address ) {
                return (uint64_t)base_address & __zone_memory_mask__( );
            }

        private:
            inline static __group_type__ page_header_group[ 4 ];
            // 辅助
        };

        uint64_t all_memory_header_count;
        uint64_t all_memory_page_desvriptor_count;

    private:
        class __address__ {
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
                        return reinterpret_cast< __helper__::__header__ * >( address );
                    }
                    else {
                        return address;
                    }
                }
                if constexpr ( __type__ == HEADER_START_ADDRESS ) {
                    return reinterpret_cast< __helper__::__header__ * >( __kheap_walker__.allocate( header_count * __helper__::header_size ) );
                }
                else {
                    if constexpr ( __mmap_allocater__ != NONE ) {
                        return (uint64_t)
                            __page_walker__.allocate< __mmap_allocater__ >(
                                PageWalker::__page_size__< __allocater_to_bind__ > * header_count * __helper__::page_descriptor_count
                                / PageWalker::__page_size__< __mmap_allocater__ > );
                    }
                    return 0ul;
                }
            }

        private:
            BOOL     has_val;
            uint64_t address;

        private:
            inline static PageWalker  __page_walker__ { };
            inline static KHeapWalker __kheap_walker__ { };
        };

    private:
        using __page_state__ = __helper__::__page_information__::__page_flags__::__page_state__;

    public:
        explicit __page_header__( IN uint64_t header_count, IN __address__ header_start_address, IN __address__ base_address ) {
            auto &group                            = __helper__::get_group( );
            this->all_memory_header_count          = header_count;
            this->all_memory_page_desvriptor_count = all_memory_header_count * __helper__::page_descriptor_count;
            this->zone                             = header_start_address.template get_address< __address__::HEADER_START_ADDRESS >( this->all_memory_header_count );
            auto base_address_                     = base_address.template get_address< __address__::BASE_ADDRESS >( this->all_memory_header_count );
            this->lock.acquire( );
            for ( auto i = 0ul; i < this->all_memory_header_count; ++i ) {
                auto &[ info, bitmap ]      = this->zone[ i ];
                info.owner                  = &this->zone[ 0 ].first;
                info.flags.state            = __page_state__::ALL_FREE;
                info.flags.type             = __allocater_to_bind__;
                info.free_memory_page_count = __helper__::page_descriptor_count;
                info.bitmap                 = &bitmap;
                info.base_address           = base_address_ + __helper__::page_descriptor_count * PageAllocater::__page_size__< __allocater_to_bind__ > * i;
                info.header_count           = 0;
                // 插入红黑树中
                info.group_node._key  = __helper__::get_keys( info.base_address );
                info.group_node._data = &info;
                group.insert( info.group_node );
            }
            this->lock.release( );
            this->zone[ 0 ].first.header_count = this->all_memory_header_count;
            this->zone[ 0 ].first.owner        = NULL;
        }
        explicit __page_header__( IN __helper__::__page_information__ *phis ) {
            this->zone = reinterpret_cast< __helper__::__header__ * >( phis );

            this->all_memory_header_count          = phis->header_count;
            this->all_memory_page_desvriptor_count = phis->header_count * __helper__::page_descriptor_count;
        }
        auto __allocate_headers__( IN uint64_t __size__ ) {
            auto &&remainder = __size__ % __helper__::page_descriptor_count;
            auto &&end       = Lib::DIV_ROUND_UP( __size__, __helper__::page_descriptor_count );
            this->lock.acquire( );
            uint64_t i { };
            while ( i < this->all_memory_header_count ) {
                auto &header = this->zone[ i ].first;
                if ( header.flags.state != __page_state__::ALL_FREE ) {
                    for ( auto j = i; j < i + end; ++j ) {
                        if ( zone[ j ].first.flags.state != __page_state__::ALL_FREE ) {
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
                auto &info                  = this->zone[ i ].first;
                info.flags.state            = __page_state__::ALL_FULL;
                info.free_memory_page_count = 0;
                info.bitmap->set( 0, __helper__::page_descriptor_count );
            }
            auto &end_info       = this->zone[ end - 1 ].first;
            end_info.flags.state = __page_state__::NORMAL;
            end_info.free_memory_page_count -= remainder;
            end_info.bitmap->set( 0, remainder );
            this->lock.release( );
        }
        auto get( IN uint64_t index ) const & -> __helper__::__header__ & {
            return this->zone[ index ];
        }

    private:
        __helper__::__header__ *zone;
        Lib::SpinLock           lock;
    };
}