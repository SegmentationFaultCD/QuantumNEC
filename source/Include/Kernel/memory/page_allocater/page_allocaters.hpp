#pragma once

#include <Lib/Uefi.hpp>
#include <Lib/list.hpp>
#include <Libcxx/bitset.hpp>
#include <bit>
#include <concepts>
#include <type_traits>
#include <utility>
#include <Kernel/print.hpp>
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
    /**
     * @brief 页内存管理器
     */
    struct PageAllocaters
    {
        explicit PageAllocaters( VOID ) noexcept = default;
        virtual ~PageAllocaters( VOID ) noexcept = default;
        /**
         * @brief 分配器
         * @param size 分配页数量
         * @retval 分配成功返回物理地址，否则返回NULL（不出错误的话，返回值永远不可能为0，因为地址0已经被占用，除非是分配失败）
         */
        virtual auto allocate( IN uint64_t size ) -> VOID * = 0;
        /**
         * @brief 释放器
         * @param address 起始物理地址
         * @param size 要释放的页数量
         */
        virtual auto free( IN VOID *address, IN uint64_t size ) -> VOID = 0;

        constexpr static auto page_size = PageSize;
    };

    /**
     * @brief 页头
     * @tparam Allocater 要绑定的目标分配器
     * @tparam InfoAllocater 要绑定的页头块分配器
     */
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
        constexpr static auto MEMORY_PAGE_DESCRIPTOR { PAGE_INFORMATION_HEADER_SIZE << 3 };     // 一个bitmap所有bit
        constexpr static auto MEMORY_PAGE_HEADER_COUNT { InfoAllocater::page_size / ( PAGE_INFORMATION_HEADER_SIZE + PAGE_BITMAP_SIZE ) };

        // 注： 一个PageHeader所指向的内存根数量便是MEMORY_PAGE_DESCRIPTOR*MEMORY_PAGE_HEADER_COUNT

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
                uint64_t type : 2;
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
        /**
         * @param pallocater 目标分配器
         * @param iallocater 页头块分配器
         * @param page_type 内存页类型
         * @param group_count 内存块组的数量
         */
        explicit PageHeader( IN Allocater &pallocater, IN InfoAllocater &iallocater, IN uint64_t page_type, IN uint64_t group_count ) :
            page_header { (header_t *)iallocater.allocate( group_count ) },     // 这样分配保证效率
            type { page_type },
            group_count { group_count },
            all_memory_header_count { MEMORY_PAGE_HEADER_COUNT * group_count },
            all_memory_page_desvriptor_count { all_memory_header_count * MEMORY_PAGE_DESCRIPTOR } {
            if ( group_count ) {     // 把group_count*InfoAllocater::page_size大小的内存块切割成为一个个header
                auto &[ head_info, head_bitmap ] = this->page_header[ 0 ];
                // 头块
                pallocater.page_header_group_list.append( head_info.group_node );     // 每个头作为节点加入目标分配器的内存块队列中
                head_info.group_node.container = &head_info;
                head_info.flags.state = PageState::ALL_FREE;
                head_info.flags.type = this->type;
                head_info.flags.physical_base = (uint64_t)virtual_to_physical( this->page_header );
                head_info.all_memory_page_count = this->MEMORY_PAGE_DESCRIPTOR;
                head_info.free_memory_page_count = this->MEMORY_PAGE_DESCRIPTOR;
                head_info.bitmap_ = &head_bitmap;
                head_info.next = reinterpret_cast< PageInformation * >( &this->page_header[ 0 ] );
                head_info.prev = nullptr;
                // 从第二块开始到结尾块之前，属于中间块
                for ( auto j = 1ul; j < this->all_memory_header_count - 1; ++j ) {
                    auto &[ mid_info, mid_bitmap ] = this->page_header[ j ];
                    if ( !( j % MEMORY_PAGE_HEADER_COUNT ) ) {     // 每个头作为节点加入目标分配器的内存块队列中
                        pallocater.page_header_group_list.append( mid_info.group_node );
                        mid_info.group_node.container = &mid_info;
                    }
                    mid_info.flags.state = PageState::ALL_FREE;
                    mid_info.flags.type = this->type;
                    mid_info.flags.physical_base = (uint64_t)virtual_to_physical( this->page_header );
                    mid_info.all_memory_page_count = this->MEMORY_PAGE_DESCRIPTOR;
                    mid_info.free_memory_page_count = this->MEMORY_PAGE_DESCRIPTOR;
                    mid_info.bitmap_ = &mid_bitmap;
                    mid_info.next = reinterpret_cast< PageInformation * >( &this->page_header[ j + 1 ] );
                    mid_info.prev = reinterpret_cast< PageInformation * >( &this->page_header[ j - 1 ] );
                }
                // 尾块
                auto &[ end_info, end_bitmap ] = this->page_header[ this->all_memory_header_count - 1 ];
                end_info.flags.state = PageState::ALL_FREE;
                end_info.flags.type = this->type;
                end_info.flags.physical_base = (uint64_t)virtual_to_physical( this->page_header );
                end_info.all_memory_page_count = this->MEMORY_PAGE_DESCRIPTOR;
                end_info.free_memory_page_count = this->MEMORY_PAGE_DESCRIPTOR;
                end_info.bitmap_ = &end_bitmap;
                end_info.next = nullptr;
                end_info.prev = reinterpret_cast< PageInformation * >( &this->page_header[ this->all_memory_header_count - 2 ] );
            }
        };
        /** 仅用于已经准备好PageInformation的情况下
         * @param phis 页头信息块组
         */
        explicit PageHeader( IN PageInformation *phis ) :
            page_header { reinterpret_cast< header_t * >( phis ) },
            group_count {
                ( [ this ]( PageInformation *phis ) -> uint64_t {
                    if ( phis && phis->group_node.container ) {     // 必须保证group_node有值，也就是头块
                        this->type = phis->flags.type;
                        uint64_t count { };
                        while ( phis ) {
                            count++;
                            phis = phis->next;
                        }
                        // 头数
                        auto header_count = !count % MEMORY_PAGE_DESCRIPTOR ? count / MEMORY_PAGE_DESCRIPTOR : Lib::DIV_ROUND_UP( count, MEMORY_PAGE_DESCRIPTOR );
                        // 组数
                        return header_count % MEMORY_PAGE_HEADER_COUNT ? header_count / MEMORY_PAGE_HEADER_COUNT + 1 : header_count / MEMORY_PAGE_HEADER_COUNT;
                    }
                    return 0;
                } )( phis )
            },
            all_memory_header_count { MEMORY_PAGE_HEADER_COUNT * group_count },
            all_memory_page_desvriptor_count { all_memory_header_count * MEMORY_PAGE_DESCRIPTOR } {
        }
        /** 第一次分配专用
         * @param pallocater 目标分配器
         * @param address 页头块分配器
         * @param page_type 内存页类型
         */
        explicit PageHeader( IN Allocater &pallocater, IN VOID *address, IN uint64_t page_type ) :
            page_header { reinterpret_cast< header_t * >( address ) },
            type { page_type },
            group_count { 1 },     // 默认数为1
            all_memory_header_count { MEMORY_PAGE_HEADER_COUNT * group_count },
            all_memory_page_desvriptor_count { all_memory_header_count * MEMORY_PAGE_DESCRIPTOR } {
            if ( group_count ) {
                auto &[ head_info, head_bitmap ] = this->page_header[ 0 ];
                pallocater.page_header_group_list.append( head_info.group_node );
                head_info.group_node.container = &head_info;
                head_info.flags.state = PageState::ALL_FREE;
                head_info.flags.type = this->type;
                head_info.flags.physical_base = (uint64_t)virtual_to_physical( this->page_header );
                head_info.all_memory_page_count = this->MEMORY_PAGE_DESCRIPTOR;
                head_info.free_memory_page_count = this->MEMORY_PAGE_DESCRIPTOR;
                head_info.bitmap_ = &head_bitmap;
                head_info.next = reinterpret_cast< PageInformation * >( &this->page_header[ 0 ] );
                head_info.prev = nullptr;
                for ( auto j = 1ul; j < all_memory_header_count - 1; ++j ) {
                    auto &[ mid_info, mid_bitmap ] = this->page_header[ j ];
                    if ( !( j % MEMORY_PAGE_HEADER_COUNT ) ) {     // 每满一组就插入进队列
                        pallocater.page_header_group_list.append( mid_info.group_node );
                        mid_info.group_node.container = &mid_info;
                    }
                    mid_info.flags.state = PageState::ALL_FREE;
                    mid_info.flags.type = this->type;
                    mid_info.flags.physical_base = (uint64_t)virtual_to_physical( this->page_header );
                    mid_info.all_memory_page_count = this->MEMORY_PAGE_DESCRIPTOR;
                    mid_info.free_memory_page_count = this->MEMORY_PAGE_DESCRIPTOR;
                    mid_info.bitmap_ = &mid_bitmap;
                    mid_info.next = reinterpret_cast< PageInformation * >( &this->page_header[ j + 1 ] );
                    mid_info.prev = reinterpret_cast< PageInformation * >( &this->page_header[ j - 1 ] );
                }
                auto &[ end_info, end_bitmap ] = this->page_header[ all_memory_header_count - 1 ];
                end_info.flags.state = PageState::ALL_FREE;
                end_info.flags.type = this->type;
                end_info.flags.physical_base = (uint64_t)virtual_to_physical( this->page_header );
                end_info.all_memory_page_count = this->MEMORY_PAGE_DESCRIPTOR;
                end_info.free_memory_page_count = this->MEMORY_PAGE_DESCRIPTOR;
                end_info.bitmap_ = &end_bitmap;
                end_info.next = nullptr;
                end_info.prev = reinterpret_cast< PageInformation * >( &this->page_header[ all_memory_header_count - 2 ] );
            }
        };

        /**
         * @brief 开辟器
         * @param allocater 辅助分配器
         * @param size 要占用内存页数量
         */
        template < typename AllocateHelper >
        auto allocate( IN AllocateHelper &allocater, IN uint64_t size ) {
            // 这样分配保证块都是连续的，而不是分散开来的
            auto base_map_address = (uint64_t)allocater.allocate( ( ( this->MEMORY_PAGE_DESCRIPTOR * Allocater::page_size ) / allocater.page_size ) * all_memory_header_count );
            this->allocate( size, base_map_address );
        }

        auto allocate( IN uint64_t size, IN uint64_t base_map_address ) {
            // 拿来取得mapbase的
            // 比如我现在是2M分配器， 有512个根
            // 那么则需要1g分配器分配一个1g来给这512个2m
            // AdvancedAllocater 便是这个1g分配器
            if ( this->group_count ) {
                // 设置每个base_map_address
                // 一共有this->all_memory_header_count个base_map_address要设置
                for ( auto i = 0ul; i < this->all_memory_header_count; ++i ) {
                    std::get< PageInformation >( this->page_header[ i ] ).map_base_adderess = base_map_address + ( this->MEMORY_PAGE_DESCRIPTOR * Allocater::page_size ) * i;
                    // 取得base_map_address的计算公式为 分配起始地址 + 内存描述符数量 * 内存页大小 * 所处页头index（这里的计算公式为组index * 内存描述符数量 + 所处组里的index）
                }
                auto remainder = size % this->MEMORY_PAGE_DESCRIPTOR;
                auto end = remainder ? size / this->MEMORY_PAGE_DESCRIPTOR + 1 : size / this->MEMORY_PAGE_DESCRIPTOR;
                for ( auto i = 0ul; i < end - 1; ++i ) {
                    // 把开头到结尾前一个都算进去
                    auto &info = std::get< PageInformation >( this->page_header[ i ] );
                    info.flags.state = PageState::ALL_FULL;
                    info.free_memory_page_count = 0;
                    info.bitmap_->set( 0, this->MEMORY_PAGE_DESCRIPTOR );
                }
                auto &end_info = std::get< PageInformation >( this->page_header[ end - 1 ] );
                end_info.flags.state = PageState::NORMAL;
                end_info.free_memory_page_count -= remainder;
                end_info.bitmap_->set( 0, remainder );
            }
        }

        auto get( IN uint64_t index ) const & -> header_t & {
            return this->page_header[ index ];
        }

    private:
        header_t *page_header;
        uint64_t type;
        uint64_t group_count;
        uint64_t all_memory_header_count { };
        uint64_t all_memory_page_desvriptor_count { };
    };
}