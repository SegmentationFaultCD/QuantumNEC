#pragma once
#include <kernel/driver/serial_port/serial_port.hpp>
#include <kernel/memory/allocator/interface.hpp>
#include <kernel/task/lock/spinlock.hpp>
#include <lib/bitset.hpp>
#include <lib/rbtree.hpp>
#include <lib/string.hpp>
#include <limine.h>
namespace Memory::Page {

auto page_memory_initialize( limine_memmap_response *map ) -> void;
enum class Type : uint64_t {
    P4Kib = 0ul,
    P2Mib = 1ul,
    P1Gib = 2ul
};
consteval auto operator""_KB( unsigned long long size ) {
    return size * 1024ul;
}
consteval auto operator""_MB( unsigned long long size ) {
    return size * 1024_KB;
}
consteval auto operator""_GB( unsigned long long size ) {
    return size * 1024_MB;
}

template < Type __type__ >
class allocator : public Memory::allocator< void > {
    friend auto page_memory_initialize( limine_memmap_response *map ) -> void;

public:
    using size_type                              = std::size_t;
    using difference_type                        = std::ptrdiff_t;
    using propagate_on_container_move_assignment = std::true_type;
    using value_type                             = void;
    using type                                   = void;

    template < class U >
    struct rebind {
        using other = allocator< __type__ >;
    };
    using self = allocator< __type__ >;

    constexpr static auto page_type = __type__;

    constexpr static auto __page_size__ = ( [] consteval {
        if constexpr ( __type__ == Type::P2Mib ) {
            return 2_MB;
        }
        else if constexpr ( __type__ == Type::P1Gib ) {
            return 1_GB;
        }
        else {
            return 4_KB;
        }
    } )( );

    constexpr static auto __page_mask__ = ~( __page_size__ - 1 );

    constexpr static auto __page_aligned__( auto &&address ) {
        return ( (uint64_t)address + __page_size__ - 1 ) & __page_mask__;
    };

    constexpr static auto __page_base__( auto &&address ) {
        return (uint64_t)address & __page_mask__;
    }
    constexpr static auto page_descriptor_count = 1024;

    struct alignas( 256 ) zone {
        // 红黑树节点
        Library::RBTree< std::uint64_t, zone * >::Node node;     // keys 计算方式最后一位zone的bitmap最后一位index(全局来看) * PAGE_SIZE + base
        // 若为单一zone，指向本身，若为多zone，指向开头zone
        zone *owner;
        // 一个zone的开头
        std::uint64_t base;
        // 多个zone的情况下连续zone的数量
        std::uint64_t zone_count;
        // 总空闲页数量（头专用）
        std::uint64_t free_page;
        // 位图
        Library::bitset< page_descriptor_count > pages;

        zone( ) = default;
        zone( std::uint64_t base_, std::uint64_t count_, Library::RBTree< std::uint64_t, zone * >::Node &&node_, std::uint64_t free_page_ ) :
            base { base_ }, zone_count { count_ }, node { node_.key( ), node_.data( ) }, free_page { free_page_ }, pages { } {
        }
    };

public:
    constexpr static auto __zone_min_memory__ = __page_size__ * page_descriptor_count;     // a zone

    constexpr static auto __zone_memory_mask_low__( std::uint64_t header_count ) {
        return ( __zone_min_memory__ * header_count - 1 );
    };
    consteval static auto __zone_memory_mask_low__( ) {
        return ( __zone_min_memory__ - 1 );
    };
    constexpr static auto __zone_memory_mask__( std::uint64_t header_count ) {
        return ~__zone_memory_mask_low__( header_count );
    };
    consteval static auto __zone_memory_mask__( ) {     // min
        return ~__zone_memory_mask_low__( );
    };

public:
    explicit allocator( void ) noexcept :
        Memory::allocator< void > { } {}
    virtual ~allocator( void ) {}

public:
    virtual auto allocate( std::size_t page_count ) -> pointer override {
        if ( page_count == 0 ) {
            return nullptr;
        }

        for ( auto zones : zone_tree[ std::to_underlying( this->page_type ) ] ) {
            if ( auto head = zones; !head->owner && head->free_page > page_count ) {
                if ( page_count <= this->page_descriptor_count ) {
                    // 对于申请数量少于一个zone所管辖的page数量的情况，只需要做一个特判，遍历这个zones组成的group中每一个zone内是否有空余即可
                    for ( auto i = 0ul; i < head->zone_count; ++i ) {
                        if ( auto result = head[ i ].pages.template find< false >( page_count ); result != -1 ) {
                            head[ i ].pages.template set< true >( result, page_count );
                            head[ i ].free_page -= page_count;
                            return reinterpret_cast< pointer >( head[ i ].base + result * __page_size__ );
                        }
                    }
                    // 单个zone查找法失败，进行多连续zone查找
                }

                // 并且对于超大的申请，将它划分为 头 躯干 尾
                // 巧妙地将躯干转化为n个zone的办法来分配
                for ( auto i = 0; i < head->zone_count; ++i ) {
                    auto head_size = head[ i ].pages.template count_from_high< false >( );

                    // 000000111111111110000000000  000000000000000000000000111
                    // low                    high  low                    high
                    //                 |                                    |
                    //                            这一块为可用区块
                    auto end_index = i + 1ul;
                    auto stop      = false;

                    for ( ; end_index < ( page_count - head_size ) / page_descriptor_count && end_index < head->zone_count; ++end_index ) {
                        if ( !head[ end_index ].pages.none( ) ) {
                            // 失败
                            stop = true;
                            break;
                        }
                    }
                    if ( stop ) {
                        continue;
                    }

                    auto end_size = ( page_count - head_size ) % page_descriptor_count;
                    if ( head[ end_index ].pages.template count_from_low< false >( ) < end_size ) {
                        continue;
                    }
                    // 有符合条件的连续zone！

                    head[ i ].pages.template set< true >( page_descriptor_count - head_size, head_size );
                    for ( auto j = i + 1; j < end_index; ++j ) {
                        head[ j ].pages.template set< true >( 0, page_descriptor_count );
                    }

                    head[ end_index ].pages.template set< true >( 0, end_size );
                    head[ i ].free_page -= page_count;
                    return reinterpret_cast< pointer >( head[ i ].base + ( page_descriptor_count - head_size ) * this->__page_size__ );

                    std::unreachable( );
                }
            }
        }
        // 两种方法都失败了
        // 这时理应开辟新zones

        auto number_of_zone = ( page_count + page_descriptor_count - 1 ) / page_descriptor_count;
        auto new_zones      = new zone[ number_of_zone ];

        std::uint64_t bases = 0;
        if constexpr ( page_type != Type::P1Gib ) {
            using Above = allocator< Type( std::to_underlying( page_type ) + 1ul ) >;
            bases       = reinterpret_cast< std::uint64_t >( Above { }.allocate( number_of_zone * this->__page_size__ * page_descriptor_count / Above::__page_size__ ) );
        }
        else {
            bases = this->global_memory_mark;
        }

        for ( auto i = 0ul; i < number_of_zone; ++i ) {
            std::construct_at( &new_zones[ i ], bases,
                               number_of_zone,
                               typename Library::RBTree< std::uint64_t, zone * >::Node( bases + std::to_underlying( this->page_type ), &new_zones[ i ] ),
                               this->page_descriptor_count );
            bases += this->page_descriptor_count * this->__page_size__;
            if constexpr ( page_type == Type::P1Gib ) {
                this->global_memory_mark += this->page_descriptor_count * this->__page_size__;
            }
            this->zone_tree[ std::to_underlying( this->page_type ) ].insert( new_zones[ i ].node );
        }
        for ( auto i = 0ul; i < number_of_zone - 1; ++i ) {
            new_zones[ i ].pages.set( 0, this->page_descriptor_count );
        }
        new_zones[ number_of_zone - 1 ].pages.set( 0, page_count % this->page_descriptor_count );
        new_zones[ 0 ].free_page = number_of_zone * this->page_descriptor_count - page_count;
        return reinterpret_cast< pointer >( new_zones[ 0 ].base );
    }
    virtual auto deallocate( const_pointer address ) -> void override {
    }

private:
    inline static Library::RBTree< std::uint64_t, zone * > zone_tree[ 3 ] { };

    inline static auto free_memory_total = 0ul;
    inline static auto all_memory_total  = 0ul;

    inline static auto global_memory_mark = 0ul;
};

}     // namespace Memory::Page