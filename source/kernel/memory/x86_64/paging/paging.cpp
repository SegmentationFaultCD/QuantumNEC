#include <kernel/cpu/cpu.hpp>
#include <kernel/global/x86_64/global.hpp>
#include <kernel/memory/page/page_allocater.hpp>
#include <kernel/memory/page/page_walker.hpp>
#include <kernel/memory/x86_64/paging/paging.hpp>
#include <kernel/memory/x86_64/paging/ptv.hpp>
#include <kernel/print.hpp>
#include <lib/shared_spinlock.hpp>
namespace QuantumNEC::Kernel::x86_64 {
pml5t pml5_t_buffer;
pml4t pml4_t_buffer;
Paging::Paging( void ) noexcept {
    this->pml5_t = new ( &pml5_t_buffer ) pml5t { };

    this->pml4_t = new ( &pml4_t_buffer ) pml4t { };

    if ( __config.paging_mode.mode == LIMINE_PAGING_MODE_X86_64_5LVL ) {
        *this->pml5_t               = (pml5t_entry *)physical_to_virtual( CPU::get_page_table( ) );
        this->support_5level_paging = true;
        this->kernel_page_table     = pml5_t;
    }
    else {
        *this->pml4_t               = (pml4t_entry *)physical_to_virtual( CPU::get_page_table( ) );
        this->support_5level_paging = false;
        this->kernel_page_table     = pml4_t;
    }
    this->kernel_page_table->page_protect( false );
}
PageAllocater allocater { };
uint64_t      address { };

using namespace std;
auto pmlxt::map( IN uint64_t physics_address, IN uint64_t virtual_address, IN uint64_t size, IN uint64_t flags, IN MemoryPageType mode ) -> void {
    pml1t  pml1t { };
    pml2t  pml2t { };
    pml3t  pml3t { };
    pml4t  pml4t { };
    pml5t  pml5t { };
    pmlxt *page_table[] { // 这里用指针数组是为了可扩展性
                          &pml1t,
                          &pml2t,
                          &pml3t,
                          &pml4t,
                          &pml5t
    };
    // 从最高级别页表开始遍历
    auto _level    = Paging::support_5level_paging ? 5 : 4;
    auto get_table = [ & ]( IN uint64_t level ) -> pmlxt & {
        return *page_table[ level - 1 ];
    };
    auto is_huge    = get_table( _level ).is_huge( mode );
    auto page_size  = get_table( _level ).check_page_size( mode );                  // 确认每页大小
    auto map_helper = [ & ]( this auto &self, uint64_t level, pmlxt &pmlx_t ) {     // 辅助函数，用于递归查找与映射
        auto index = pmlx_t.get_address_index_in( reinterpret_cast< void * >( virtual_address ) );
        if ( level == static_cast< uint64_t >( mode ) ) {
            // 当前等级符合分页模式
            // 如，2m分页，那么每个2级页表就是最底层
            // 换句话说，就是舍去1级页表
            // 4K分页，那么就是查到1级页表为止
            // 1G分页，就是查到3级页表为止
            if ( mode == PAGE_1G ) {
                if ( pmlx_t.flags_ps_pat( index ) || !pmlx_t.get_table( )[ index ] ) {
                    goto normal;
                }
                else {
                    get_table( PAGE_2M )   = (uint64_t)physical_to_virtual( pmlx_t.flags_base( index, PAGE_4K ) );
                    pmlxt &next_page_table = get_table( PAGE_2M );
                    for ( auto i = 0; i < 512; ++i ) {
                        if ( !next_page_table.flags_ps_pat( i ) || next_page_table.get_table( )[ i ] ) {
                            PageWalker { }.free< MemoryPageType::PAGE_4K >( (void *)next_page_table.flags_base( i, PAGE_4K ), 1 );
                        }
                    }
                    PageWalker { }.free< MemoryPageType::PAGE_4K >( (void *)pmlx_t.flags_base( index, PAGE_4K ), 1 );
                }
            }
            if ( mode == PAGE_2M ) {
                if ( pmlx_t.flags_ps_pat( index ) || pmlx_t.get_table( )[ index ] ) {
                    goto normal;
                }
                else {
                    PageWalker { }.free< MemoryPageType::PAGE_4K >( (void *)pmlx_t.flags_base( index, PAGE_4K ), 1 );
                }
            }

        normal:     // 如果为huge页那么设置ps位为1
            pmlx_t = { index, physics_address & ~0x7FFul, flags | is_huge, mode };
            // CPU::invlpg( reinterpret_cast< void * >( virtual_address ) );     // 刷新快表
            physics_address += page_size;
            virtual_address += page_size;
            return;
        }
        // Q:为啥判断页存在要放在判断级别符合模式后面？
        // A:假设我们现在在3级页表中，这个3级页表中第index个项可能p位被置0,或者压根就没有指向2级页表
        // 情况为前者时，一般分页模式会和页表级别符合，我们可以直接用他
        // 情况为后者时，一般分页模式会和页表级别不符合，那么就要新建一个页表，然后继续迭代，迭代到2级页表时，我们就可以使用他了
        // 当然还有第三种情况，页p位为1且指向二级页表，
        // 这种情况下，那么就是继续迭代既可

        else if ( !pmlx_t.flags_p( index ) || pmlx_t.flags_ps_pat( index ) ) {
            auto new_ = allocater.allocate< MemoryPageType::PAGE_4K >( 1 );
            std::memset( physical_to_virtual( new_ ), 0, pmlx_t.PT_SIZE );
            pmlx_t = { index, ( reinterpret_cast< uint64_t >( new_ ) & ~0x7FFul ), flags, PAGE_4K };
        }
        get_table( level - 1 ) = (uint64_t)physical_to_virtual( pmlx_t.flags_base( index, PAGE_4K ) );     // 得到下一级页表的地址
        self( level - 1, get_table( level - 1 ) );
    };

    while ( size-- ) {     // 重复循环映射
        map_helper( _level, *this );
    }
}
auto pmlxt::unmap( IN uint64_t virtual_address, IN size_t size, IN MemoryPageType mode ) -> void {
    pml1t  pml1t { };
    pml2t  pml2t { };
    pml3t  pml3t { };
    pml4t  pml4t { };
    pml5t  pml5t { };
    pmlxt *page_table[] {
        &pml1t,
        &pml2t,
        &pml3t,
        &pml4t,
        &pml5t
    };
    auto _level    = Paging::support_5level_paging ? 5 : 4;
    auto get_table = [ & ]( IN uint64_t level ) -> pmlxt & {
        return *page_table[ level - 1 ];
    };
    auto page_size = pml4t.check_page_size( mode );
    // 以上基本与map一致
    auto unmap_helper = [ & ]( this auto &self, uint64_t level, pmlxt &pmlx_t ) {     // 辅助函数
        auto index = pmlx_t.get_address_index_in( reinterpret_cast< void * >( virtual_address ) );
        if ( !pmlx_t.flags_p( index ) ) {     // 找到但是本身就不存在，那么直接返回既可
            virtual_address += page_size;
            return;
        }
        // Q:为什么这里要将判断p位放在前面？
        // A:为了节约性能，
        // 在前面先继续判断，例如我要取消映射的虚拟地址在2级页表里面，但这个虚拟地址在三级页表中的项就不指向任何2级页表，那么就没必要继续找了
        // 再比如，这个虚拟地址就在这个3级页表里面，但是p位已经被置为0，那么就没必要像下面这样做了
        // unmap本身就是让虚拟地址所在页的p位变成0嘛。。。
        else if ( level == static_cast< uint64_t >( mode ) ) {
            pmlx_t.set_p( index, 0 );
            CPU::invlpg( reinterpret_cast< void * >( virtual_address ) );
            virtual_address += page_size;
            return;
        }
        get_table( level - 1 ) = (uint64_t)physical_to_virtual( pmlx_t.flags_base( index, mode ) );
        self( level - 1, get_table( level - 1 ) );
    };
    while ( size-- ) {
        unmap_helper( _level, *this );
    }
}

auto pmlxt::VTP_from( IN void *virtual_address, IN MemoryPageType mode ) -> void * {
    pml1t  pml1t { };
    pml2t  pml2t { };
    pml3t  pml3t { };
    pml4t  pml4t { };
    pml5t  pml5t { };
    pmlxt *page_table[] {
        &pml1t,
        &pml2t,
        &pml3t,
        &pml4t,
        &pml5t
    };
    auto level     = Paging::support_5level_paging ? 5 : 4;
    auto get_table = [ & ]( IN uint64_t level ) -> pmlxt & {
        return *page_table[ level - 1 ];
    };
    auto offset { 0ul };
    if ( mode == MemoryPageType::PAGE_2M ) {
        offset = 1;
    }
    else if ( mode == MemoryPageType::PAGE_1G ) {
        offset = 2;
    }

    auto find_helper = [ &get_table, &offset, &mode ]( this auto &self, IN void *virtual_address, IN uint64_t level, IN pmlxt &pmlx_t ) -> uint64_t * {
        auto index { pmlx_t.get_address_index_in( virtual_address ) };
        if ( !pmlx_t.flags_p( index ) ) {
            return nullptr;
        }
        if ( !( ( level - offset ) - 1 ) ) {
            auto entry = (uint64_t)physical_to_virtual( pmlx_t.flags_base( index, mode ) );
            return (uint64_t *)virtual_to_physical( entry );
        }
        else {
            auto entry             = (uint64_t)physical_to_virtual( pmlx_t.flags_base( index, MemoryPageType::PAGE_4K ) );
            get_table( level - 1 ) = entry;
            return self( virtual_address, level - 1, get_table( level - 1 ) );
        }
    };
    return find_helper( virtual_address, level, *this );
}
auto pmlxt::page_protect( IN bool flags ) -> void {
    auto cr0 = CPU::read_cr0( );
    if ( !flags ) {
        cr0.WP = 1;
        CPU::write_cr0( cr0 );
    }
    else {
        cr0.WP = 0;
        CPU::write_cr0( cr0 );
    }
};
auto pmlxt::make( IN uint64_t flags, IN Level level, IN MemoryPageType mode ) -> pmlxt & {
    pml1t  pml1t { };
    pml2t  pml2t { };
    pml3t  pml3t { };
    pml4t  pml4t { };
    pml5t  pml5t { };
    pmlxt *page_table[] {
        &pml1t,
        &pml2t,
        &pml3t,
        &pml4t,
        &pml5t
    };

    auto get_table = [ & ]( IN uint64_t level ) -> pmlxt & {
        return *page_table[ level - 1 ];
    };
    auto tlevel { static_cast< uint64_t >( level ) };
    auto offset { 0ul };
    auto taddress { 0ul };

    if ( mode == MemoryPageType::PAGE_2M ) {
        offset = 1;
        // 舍去1级页表
    }
    else if ( mode == MemoryPageType::PAGE_1G ) {
        offset = 2;
        // 舍去2级页表和1级页表
    }
    *this            = (uint64_t)allocater.allocate< MemoryPageType::PAGE_4K >( 1 );
    auto later_flags = flags | this->is_huge( mode );
    auto page_size   = this->check_page_size( mode );
    auto make_helper = [ & ]( this auto &self, IN pmlxt &pmlx_t ) -> void {
        if ( ( tlevel - offset ) - 1 ) {
            // 只要没到底，那么就给一块2M内存，存512个4k页表
            taddress = (uint64_t)allocater.allocate< MemoryPageType::PAGE_2M >( 1 );
        }
        for ( auto i { 0ul }; i < 512; ++i ) {
            // ( tlevel - offset ) - 1为0的时候说明到底了
            if ( !( ( tlevel - offset ) - 1 ) ) {
                // 到底了，像2级页表每个项指向一个2M内存，
                // 1级页表每个项指向一个4K内存，
                // 并赋予属性，将整个页表填充
                pmlx_t = { i, address, later_flags, mode };
                address += page_size;
            }
            else {
                pmlx_t                = { i, taddress, flags, mode };
                get_table( --tlevel ) = taddress;
                taddress += 512;
                // 还没到底，那么说明还要继续迭代，到低一级页表
                self( get_table( tlevel ) );
                // 每次调用完，就算是一个项给填充了，
                // 再次循环直至512个项目都被填充
            }
        }
        tlevel++;     // 这个是全局的，如果最这一级页表迭代所有项填充完成，那就返回上一级页表，一直重复
        return;
    };
    make_helper( *this );
    return *this;
}
auto pmlxt::activate( void ) -> void {
    if ( this->get_table( ) ) {
        CPU::set_page_table( (uint64_t *)virtual_to_physical( this->get_table( ) ) );
    }
}
auto pmlxt::copy( IN pmlxt &from ) -> void {
    auto page_table = (uint64_t *)physical_to_virtual( PageWalker { }.allocate< MemoryPageType::PAGE_4K >( 1 ) );
    // copy high 2048 size.
    std::memset( page_table, 0, PT_SIZE );
    std::memcpy( page_table + 256, from.get_table( ) + 256, PT_SIZE / 2 );
    *this = (uint64_t)page_table;
}
}     // namespace QuantumNEC::Kernel::x86_64
