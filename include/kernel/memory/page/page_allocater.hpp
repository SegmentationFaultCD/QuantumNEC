#pragma once
#include <kernel/memory/heap/kheap/kheap_allocater.hpp>
#include <kernel/memory/memory.hpp>
#include <kernel/memory/page/page_manager.hpp>
#include <lib/Uefi.hpp>
#include <lib/rbtree.hpp>
#include <lib/shared_spinlock.hpp>
#include <libcxx/bitset.hpp>
#include <utility>
namespace QuantumNEC::Kernel {
class PageAllocater {
public:
    template < MemoryPageType __type__ >
        requires( __type__ != NONE )
    constexpr static auto __page_size__ = ( [] consteval {
        if constexpr ( __type__ == MemoryPageType::PAGE_2M ) {
            return 2_MB;
        }
        else if constexpr ( __type__ == MemoryPageType::PAGE_1G ) {
            return 1_GB;
        }
        else {
            return 4_KB;
        }
    } )( );
    template < MemoryPageType __type__ >
        requires( __type__ != MemoryPageType::NONE )
    constexpr static auto __page_mask__ = ~( __page_size__< __type__ > - 1 );
    template < MemoryPageType __type__ >
        requires( __type__ != MemoryPageType::NONE )
    constexpr static auto __page_aligned__( auto &&address ) {
        return ( (uint64_t)address + __page_size__< __type__ > - 1 ) & __page_mask__< __type__ >;
    };
    template < MemoryPageType __type__ >
    constexpr static auto __page_base__( IN auto &&address ) {
        return (uint64_t)address & __page_mask__< __type__ >;
    }
    template < MemoryPageType __type__ >
    auto allocate( IN uint64_t __size__ );

public:
    template < MemoryPageType __allocater_to_bind__, MemoryPageType __mmap_allocater__ >
        requires( __allocater_to_bind__ != NONE )
    class __page_header__ {
    public:
        class __helper__ {
            constexpr static auto page_information_block_size { 256 };

        public:
            constexpr static auto page_descriptor_count { 1024 };     // 一个bitmap所有bit

            struct __page_information__;

            using __node_key__   = uint64_t;
            using __group_type__ = RedBlackTree< __page_information__, __node_key__ >;
            using __node_type__  = __group_type__::RedBlackTreeNode;
            struct _aligned( page_information_block_size ) __page_information__ {
                __node_type__ group_node;     // 连接每个页头
                struct __page_flags__ {
                    enum class __page_state__ : uint64_t {
                        ALL_FULL = 0,
                        ALL_FREE = 1,
                        NORMAL   = 2
                    };
                    __page_state__ state : 2;
                    uint64_t       type : 2;
                    uint64_t : 60;

                    explicit __page_flags__( void ) noexcept = default;

                } flags;                                                           // 标志
                uint64_t                                         base_address;     // 记录块起始地址,这个也是Key
                uint64_t                                         free_memory_page_count;
                uint64_t                                         header_count;     // 一个区域所包含的头数
                __page_information__                            *owner;
                std::bitset< __helper__::page_descriptor_count > bitmap;
                // 其中
                // (Zone‘s memory total)normal = header_count * page_descriptor_count * page size
                // (Zone‘s memory total)min = page_descriptor_count * page size, 也就是只有一个页

                explicit __page_information__( void ) = default;
            };

            constexpr static auto header_size { sizeof( __page_information__ ) };

        public:
            constexpr static auto __zone_min_memory__ = PageAllocater::__page_size__< __allocater_to_bind__ > * page_descriptor_count;     // a zone

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
            static auto get_group( void ) -> Lib::shared_spinlock< __group_type__ > & {
                return page_header_group[ __allocater_to_bind__ ];
            }
            static auto get_keys( IN auto base_address ) {
                return (uint64_t)base_address & __zone_memory_mask__( );
            }

        private:
            inline static Lib::shared_spinlock< __group_type__ > page_header_group[ 4 ];
            // 辅助
        };

        uint64_t all_memory_header_count;

    private:
        class __address__ {
        public:
            enum __address_type__ {
                BASE_ADDRESS,
                HEADER_START_ADDRESS
            };

        public:
            __address__( void ) :
                has_val { false },
                address { 0 } {
            }
            __address__( uint64_t address ) :
                has_val { true },
                address { address } {
            }
            template < __address_type__ __type__ >
            auto get_address( IN uint64_t header_count ) {
                if ( has_val ) {
                    if constexpr ( __type__ == HEADER_START_ADDRESS ) {
                        return reinterpret_cast< __helper__::__page_information__ * >( address );
                    }
                    else {
                        return address;
                    }
                }
                if constexpr ( __type__ == HEADER_START_ADDRESS ) {
                    return reinterpret_cast< __helper__::__page_information__ * >( KHeapAllocater { }.allocate( header_count * __helper__::header_size ) );
                }
                else {
                    if constexpr ( __mmap_allocater__ != NONE ) {
                        return (uint64_t)PageAllocater { }.allocate< __mmap_allocater__ >( PageAllocater::__page_size__< __allocater_to_bind__ > * header_count * __helper__::page_descriptor_count / PageAllocater::__page_size__< __mmap_allocater__ > );
                    }
                    return 0ul;
                }
            }

        private:
            bool     has_val;
            uint64_t address;
        };

    private:
        using __page_state__ = __helper__::__page_information__::__page_flags__::__page_state__;

    public:
        explicit __page_header__( IN uint64_t header_count, IN __address__ header_start_address, IN __address__ base_address ) {
            auto &group                   = __helper__::get_group( );
            this->all_memory_header_count = header_count;
            this->zone                    = header_start_address.template get_address< __address__::HEADER_START_ADDRESS >( this->all_memory_header_count );
            auto base_address_            = base_address.template get_address< __address__::BASE_ADDRESS >( this->all_memory_header_count );

            group.visit(
                [ &, this ]( const Lib::shared_spinlock< typename __helper__::__group_type__ > &group ) {
                    for ( auto i = 0ul; i < this->all_memory_header_count; ++i ) {
                        this->zone[ i ].owner                  = &this->zone[ 0 ];
                        this->zone[ i ].flags.state            = __page_state__::ALL_FREE;
                        this->zone[ i ].flags.type             = __allocater_to_bind__;
                        this->zone[ i ].free_memory_page_count = __helper__::page_descriptor_count;
                        this->zone[ i ].base_address           = base_address_ + __helper__::page_descriptor_count * PageAllocater::__page_size__< __allocater_to_bind__ > * i;
                        this->zone[ i ].header_count           = 0;
                        // 插入红黑树中
                        this->zone[ i ].group_node._key  = __helper__::get_keys( this->zone[ i ].base_address );
                        this->zone[ i ].group_node._data = &this->zone[ i ];
                        group.value( ).insert( this->zone[ i ].group_node );
                    }
                } );
            this->zone[ 0 ].header_count = this->all_memory_header_count;
            this->zone[ 0 ].owner        = NULL;
        }
        explicit __page_header__( IN __helper__::__page_information__ *phis ) {
            this->zone = reinterpret_cast< __helper__::__page_information__ * >( phis );
        }
        // Be used to set headers when users create page headers.
        auto __allocate_headers__( IN uint64_t __size__ ) && -> __page_header__ & {
            auto &&remainder = __size__ % __helper__::page_descriptor_count;
            auto &&end       = Lib::DIV_ROUND_UP( __size__, __helper__::page_descriptor_count );

            for ( auto i = 0ul; i < end - 1; ++i ) {
                // 把开头到结尾前一个都算进去
                this->zone[ i ].flags.state            = __page_state__::ALL_FULL;
                this->zone[ i ].free_memory_page_count = 0;
                this->zone[ i ].bitmap.set( 0, __helper__::page_descriptor_count );
            }
            this->zone[ end - 1 ].flags.state = __page_state__::NORMAL;
            this->zone[ end - 1 ].free_memory_page_count -= remainder;
            this->zone[ end - 1 ].bitmap.set( 0, remainder );

            return *this;
        }
        auto operator[]( IN uint64_t index ) const -> __helper__::__page_information__ & {
            return this->zone[ index ];
        }

    private:
        __helper__::__page_information__ *zone;
    };
    friend PageManager;

public:
    explicit PageAllocater( void ) noexcept = default;
    virtual ~PageAllocater( void ) noexcept = default;

public:
    // 分配器
    template < MemoryPageType __type__ >
        requires( __type__ != MemoryPageType::NONE )
    auto allocate( IN uint64_t __size__ ) {
        using PH  = __page_header__< __type__, ( [ & ] consteval {
                                        if constexpr ( __type__ == PAGE_1G )
                                            return NONE;
                                        else
                                            return MemoryPageType( __type__ + 1 );
                                    } )( ) >;
        using PHI = PH::__helper__::__page_information__;
        auto &group { PH::__helper__::get_group( ) };

        auto index                   = 0ul;
        auto bitmap_index            = 0ul;
        auto first_header_bits_count = 0ul;
        // The variable is not always used.
        // If size is less than page descriptor count of a header, the variable will not be used.
        auto needed_header_count = Lib::DIV_ROUND_UP( __size__, PH::__helper__::page_descriptor_count );
        return group.visit( [ &, this ]( const Lib::shared_spinlock< typename PH::__helper__::__group_type__ > &group ) -> void * {
            PHI *node { };
            if ( __size__ < PH::__helper__::page_descriptor_count ) {
                if ( !group.value( ).empty( ) ) {
                    group.value( ).traverse( [ & ]( PHI &zone ) {
                        if ( zone.owner ) {
                            return false;
                        }
                        // zone.owner is null, that means it is the head of the zone.
                        auto header  = &zone;
                        auto success = false;
                        for ( auto i = 0ul; i < zone.header_count; ++i ) {
                            // If the header's state is ALL_FULL, that means there is no need to search the header's bitmap.
                            // Because the bitmap's the all of bits are filled, it also means there is no memory can be allocated from the memory space managed by the header.
                            if ( header[ i ].flags.state != PHI::__page_flags__::__page_state__::ALL_FULL ) {
                                // If the zone's state isn't ALL_FULL, that means we can allocate memory from the memory space managed by the header.
                                // Than, we should find a free bit from bitmap.
                                if ( auto result = header[ i ].bitmap.template find< false >( __size__ ); result.has_value( ) ) {
                                    index                   = i;
                                    bitmap_index            = result.value( );
                                    node                    = &zone;
                                    success                 = true;
                                    first_header_bits_count = __size__;
                                    // Find the suitable zone to allocate, jump.
                                    break;
                                }
                            }
                            else {
                                // switch to the next header.
                                continue;
                            }
                        }
                        if ( success ) {
                            return true;
                        }
                        return false;
                    } );
                }
            }
            else {
                if ( !group.value( ).empty( ) ) {
                    group.value( ).traverse( [ & ]( PHI &zone ) {
                        if ( zone.owner ) {
                            return false;
                        }
                        // Pass, it means the header is the first header in the zone.
                        if ( zone.header_count < needed_header_count ) {
                            return false;
                        }
                        // Pass, it means the number of headers in the zone satisfies needed headers.
                        // Then, translate the zone so that searching suitable and uninterrupted headers group.
                        auto header  = &zone;
                        auto success = true;
                        auto i       = 0ul;
                        for ( ; i < zone.header_count; ++i ) {
                            // The basicest condition, the range from i to (i + needed_header_count - 1) must be between 0 and zone.header_count - 1.
                            if ( i + needed_header_count > zone.header_count ) {
                                success = false;
                                break;
                            }
                            // The second condition, the state in the first header isn't ALL_FULL.
                            if ( header[ i ].flags.state == PHI::__page_flags__::__page_state__::ALL_FULL ) {
                                // the header has no bit can filled;
                                continue;
                            }
                            // The third confition, the number of uninterrupted bits from the highest to meet the first filled bit in the first header must be greater than 0
                            first_header_bits_count = header[ i ].bitmap.template count_from_high< false >( );
                            if ( !first_header_bits_count ) {
                                // the header has no uninterrupted and free bit
                                continue;
                            }
                            // The fourth confition, all status in the headers from i + 1 to (i + middle_header_count - 1) are ALL_FREE
                            auto new_size            = __size__ - first_header_bits_count;
                            auto middle_header_count = new_size / PH::__helper__::page_descriptor_count;
                            for ( auto j = i + 1; j < i + middle_header_count; ++j ) {
                                if ( header[ j ].flags.state != PHI::__page_flags__::__page_state__::ALL_FREE ) {
                                    // Fail, Someone's state isn't ALL_FREE
                                    success = false;
                                    break;
                                }
                            }
                            if ( !success ) {
                                continue;
                            }
                            // The fifth condition, the state in the final header isn't ALL_FULL.
                            if ( header[ i + middle_header_count ].flags.state == PHI::__page_flags__::__page_state__::ALL_FULL ) {
                                // the header has no bit can filled;
                                continue;
                            }
                            // The final confition, the number of uninterrupted bits from the lowest to meet the first filled bit in final header must be greater than remainder_size
                            auto remainder_size          = new_size % PH::__helper__::page_descriptor_count;
                            auto final_header_bits_count = header[ i + middle_header_count ].bitmap.template count_from_low< false >( );
                            if ( final_header_bits_count < remainder_size ) {
                                // the header has no uninterrupted and free bit
                                continue;
                            }
                            success = true;
                            break;
                        }
                        if ( !success ) {
                            return false;
                        }
                        // Successfully find suitable headers group.
                        node         = &zone;
                        index        = i;
                        bitmap_index = header[ i ].bitmap.template find_from_high< false >( );
                        return true;
                    } );
                }
            }

            if ( node ) {
                // There are some suitable page headers searched.

                // head
                node[ index ].bitmap.set( bitmap_index, first_header_bits_count );
                node[ index ].free_memory_page_count -= first_header_bits_count;
                if ( !node[ index ].free_memory_page_count ) {
                    node[ index ].flags.state = PHI::__page_flags__::__page_state__::ALL_FULL;
                }
                else {
                    node[ index ].flags.state = PHI::__page_flags__::__page_state__::NORMAL;
                }
                if ( __size__ >= PH::__helper__::page_descriptor_count ) {
                    // middle
                    for ( auto i = index + 1; i < index + needed_header_count - 1; ++i ) {
                        node[ i ].bitmap.set( 0, PH::__helper__::page_descriptor_count );
                        node[ i ].free_memory_page_count = 0;
                        node[ i ].flags.state            = PHI::__page_flags__::__page_state__::ALL_FULL;
                    }
                    // end
                    node[ index + needed_header_count - 1 ].free_memory_page_count -= __size__ % PH::__helper__::page_descriptor_count;
                    node[ index + needed_header_count - 1 ].bitmap.set( 0, __size__ % PH::__helper__::page_descriptor_count );
                    if ( !node[ index + needed_header_count - 1 ].free_memory_page_count ) {
                        node[ index + needed_header_count - 1 ].flags.state = PHI::__page_flags__::__page_state__::ALL_FULL;
                    }
                    else {
                        node[ index + needed_header_count - 1 ].flags.state = PHI::__page_flags__::__page_state__::NORMAL;
                    }
                }

                auto address = node[ index ].base_address + bitmap_index * this->__page_size__< __type__ >;
                std::memset( (void *)physical_to_virtual( address ), 0, __size__ * this->__page_size__< __type__ > );
                return (void *)address;
            }
            if constexpr ( __type__ == PAGE_1G ) {
                // No any zone is suitable
                // Create a new zone to allocate.
                auto address = (void *)( PH { needed_header_count, { }, global_memory_address }.__allocate_headers__( __size__ )[ 0 ].base_address );
                // 1G allocate need control Global Memory Address
                // Beacuse it doesn't have any mmap allocater.
                // So it has to control memory address(memory map) by itself.
                global_memory_address += needed_header_count * PH::__helper__::__zone_min_memory__;
                std::memset( (void *)physical_to_virtual( address ), 0, __size__ * this->__page_size__< __type__ > );
                return address;
            }
            else {
                // No any zone is suitable
                // Create a new zone to allocate.
                auto address = (void *)( PH { needed_header_count, { }, {} }.__allocate_headers__( __size__ )[ 0 ].base_address );
                std::memset( (void *)physical_to_virtual( address ), 0, __size__ * this->__page_size__< __type__ > );
                return address;
            }
        } );
    }
    // 这个操作不会初始化分配的空间，需要手动清零
private:
    // Global memory address is used by 1G allocater
    inline static uint64_t global_memory_address { };
};

}     // namespace QuantumNEC::Kernel