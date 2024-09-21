#include <Kernel/memory/memory.hpp>
#include <Arch/Arch.hpp>
#include <Libcxx/string.hpp>
#include <Lib/spin_lock.hpp>
#include <Kernel/print.hpp>
PUBLIC namespace {
    using namespace QuantumNEC::Lib;
    using namespace std;
    using namespace QuantumNEC;
    using namespace QuantumNEC::Kernel;
    PRIVATE constexpr auto FONT_FILE_OCCUPIES_PAGE { 16 };
    PRIVATE auto address { 0ul };
    PRIVATE SpinLock lock { };
}

PUBLIC namespace QuantumNEC::Kernel {
    auto PagingMap::VTP_address_from( IN VOID * virtual_address, IN MemoryPageType mode, IN pmlxt & pmlx_t ) -> VOID * {
        lock.acquire( );
        pml1t pml1t { };
        pml2t pml2t { mode };
        pml3t pml3t { mode };
        pml4t pml4t { };
        pml5t pml5t { };
        pmlxt *page_table[] {
            &pml1t,
            &pml2t,
            &pml3t,
            &pml4t,
            &pml5t
        };
        auto level = this->support_5level_paging ? 5 : 4;
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

        auto find_helper = [ &get_table, &offset ]( this auto &self, IN VOID *virtual_address, IN uint64_t level, IN pmlxt &pmlx_t ) -> uint64_t * {
            auto index { pmlx_t.get_address_index_in( virtual_address ) };
            if ( !pmlx_t.flags_p( index ) ) {
                return nullptr;
            }
            auto entry = (uint64_t)physical_to_virtual( pmlx_t.flags_base( index ) );
            if ( !( ( level - offset ) - 1 ) ) {
                return (uint64_t *)entry;
            }

            get_table( level - 1 ) = entry;
            return self( virtual_address, level - 1, get_table( level - 1 ) );
        };
        lock.release( );
        return find_helper( virtual_address, level, pmlx_t );
    }
    using namespace Architecture;

    PagingMap::PagingMap( VOID ) noexcept :
        kernel_l1_page_table { },
        kernel_l2_page_table { MemoryPageType::PAGE_2M },
        kernel_l3_page_table { MemoryPageType::PAGE_2M },
        kernel_l4_page_table { },
        kernel_l5_page_table { } {
        if ( Architecture::__config.paging_mode.mode == LIMINE_PAGING_MODE_X86_64_5LVL ) {
            kernel_l5_page_table = (pml5t_entry *)physical_to_virtual( ArchitectureManager< TARGET_ARCH >::get_page_table( ) );
            this->kernel_page_table = &kernel_l5_page_table;
        }
        else {
            kernel_l4_page_table = (pml4t_entry *)physical_to_virtual( ArchitectureManager< TARGET_ARCH >::get_page_table( ) );
            this->kernel_page_table = &kernel_l4_page_table;
        }

        //  消除页保护
        this->page_protect( FALSE );
        // 映射
        // for ( auto page_address { 0ul }; page_address < Memory::page->free_memory_total; page_address += PAGE_2M_SIZE ) {     // 这里为什么不用all_memory_total?因为它超过物理内存大小了
        //     this->map( page_address, (uint64_t)physical_to_virtual( page_address ), 1, PAGE_PRESENT | PAGE_RW_W | PAGE_US_U, Mode::MEMORY_MAP_2M, kernel_l4_page_table );
        // }
    }

    auto PagingMap::map( IN uint64_t physics_address, IN uint64_t virtual_address, IN uint64_t size, IN uint64_t flags, IN MemoryPageType mode, IN pmlxt & _pmlxt ) -> VOID {
        lock.acquire( );

        pml1t pml1t { };
        pml2t pml2t { mode };
        pml3t pml3t { mode };
        pml4t pml4t { };
        pml5t pml5t { };
        pmlxt *page_table[] { // 这里用指针数组是为了可扩展性
                              &pml1t,
                              &pml2t,
                              &pml3t,
                              &pml4t,
                              &pml5t
        };
        // 从最高级别页表开始遍历
        auto _level = this->support_5level_paging ? 5 : 4;
        auto get_table = [ & ]( IN uint64_t level ) -> pmlxt & {
            return *page_table[ level - 1 ];
        };
        auto page_size = pml4t.check_page_size( mode );                                                    // 确认每页大小
        flags |= pml4t.is_huge( mode );                                                                    // 如果为huge页那么设置ps位为1
        auto map_helper = [ & ]( this auto &self, uint64_t level, pmlxt &pmlx_t ) {                        // 辅助函数，用于递归查找与映射
            auto index = pmlx_t.get_address_index_in( reinterpret_cast< VOID * >( virtual_address ) );     // 拿到虚拟地址所在页表入口的index

            if ( level == static_cast< uint64_t >( mode ) ) {
                // 当前等级符合分页模式
                // 如，2m分页，那么每个2级页表就是最底层
                // 换句话说，就是舍去1级页表
                // 4K分页，那么就是查到1级页表为止
                // 1G分页，就是查到3级页表为止

                pmlx_t = { index, physics_address & ~0x7FF, flags & 0x7FF };
                Architecture::ArchitectureManager< TARGET_ARCH >::invlpg( reinterpret_cast< VOID * >( virtual_address ) );     // 刷新快表
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
            else if ( !pmlx_t.flags_p( index ) ) {
                // 这个页要是是一个不存在的那么就弄出一个4k大小表给他
                auto new_ = Memory::page->allocate( 1, MemoryPageType::PAGE_4K );
                pmlx_t = { index, ( reinterpret_cast< uint64_t >( new_ ) & ~0x7FF ), flags };
                memset( physical_to_virtual( new_ ), 0, page_size );
            }

            get_table( level - 1 ) = (uint64_t)physical_to_virtual( pmlx_t.flags_base( index ) );     // 得到下一级页表的地址
            self( level - 1, get_table( level - 1 ) );
        };
        while ( size-- ) {     // 重复循环映射
            map_helper( _level, _pmlxt );
        }
        lock.release( );
    }
    auto PagingMap::unmap( IN uint64_t virtual_address, IN size_t size, IN MemoryPageType mode, IN pmlxt & _pmlxt ) -> VOID {
        lock.acquire( );
        pml1t pml1t { };
        pml2t pml2t { mode };
        pml3t pml3t { mode };
        pml4t pml4t { };
        pml5t pml5t { };
        pmlxt *page_table[] {
            &pml1t,
            &pml2t,
            &pml3t,
            &pml4t,
            &pml5t
        };
        auto _level = this->support_5level_paging ? 5 : 4;
        auto get_table = [ & ]( IN uint64_t level ) -> pmlxt & {
            return *page_table[ level - 1 ];
        };
        auto page_size = pml4t.check_page_size( mode );
        // 以上基本与map一致
        auto unmap_helper = [ & ]( this auto &self, uint64_t level, pmlxt &pmlx_t ) {     // 辅助函数
            auto index = pmlx_t.get_address_index_in( reinterpret_cast< VOID * >( virtual_address ) );
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
                Memory::page->free( (VOID *)pmlx_t.flags_base( index ), 1, mode );
                Architecture::ArchitectureManager< TARGET_ARCH >::invlpg( reinterpret_cast< VOID * >( virtual_address ) );
                virtual_address += page_size;
                return;
            }
            get_table( level - 1 ) = (uint64_t)physical_to_virtual( pmlx_t.flags_base( index ) );
            self( level - 1, get_table( level - 1 ) );
        };
        while ( size-- ) {
            unmap_helper( _level, _pmlxt );
        }

        lock.release( );
    }

    auto PagingMap::page_protect( IN BOOL flags ) -> VOID {
        lock.acquire( );
        auto cr0 = Architecture::ArchitectureManager< TARGET_ARCH >::read_cr0( );
        if ( !flags ) {
            cr0.WP = 1;
            Architecture::ArchitectureManager< TARGET_ARCH >::write_cr0( cr0 );
            println< ostream::HeadLevel::SYSTEM >( "Disable the page protection." );
        }
        else {
            cr0.WP = 0;
            Architecture::ArchitectureManager< TARGET_ARCH >::write_cr0( cr0 );
            println< ostream::HeadLevel::SYSTEM >( "Enable the page protection." );
        }
        lock.release( );
    };
    auto PagingMap::make( IN uint64_t flags, IN Level level, IN MemoryPageType mode, IN pmlxt & pmlx_t ) -> pmlxt & {
        lock.acquire( );
        pml1t pml1t { };
        pml2t pml2t { mode };
        pml3t pml3t { mode };
        pml4t pml4t { };
        pml5t pml5t { };
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
        // 外部只是丢进来一个放页表地址的容器而已，他不指向任何页表，所以我们要先给他一个4k内存页当成页表
        pmlx_t = (uint64_t)Memory::page->allocate( 1, MemoryPageType::PAGE_4K );
        auto later_flags = flags | pmlx_t.is_huge( mode );
        auto page_size = pmlx_t.check_page_size( mode );
        auto make_helper = [ & ]( this auto &self, IN pmlxt &pmlx_t ) -> VOID {
            if ( ( tlevel - offset ) - 1 ) {
                // 只要没到底，那么就给一块2M内存，存512个4k页表
                taddress = (uint64_t)Memory::page->allocate( 1, MemoryPageType::PAGE_2M );
            }
            for ( auto i { 0ul }; i < 512; ++i ) {
                // ( tlevel - offset ) - 1为0的时候说明到底了
                if ( !( ( tlevel - offset ) - 1 ) ) {
                    // 到底了，像2级页表每个项指向一个2M内存，
                    // 1级页表每个项指向一个4K内存，
                    // 并赋予属性，将整个页表填充
                    pmlx_t = { i, address, later_flags };
                    address += page_size;
                }
                else {
                    pmlx_t = { i, taddress, flags };
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
        make_helper( pmlx_t );
        lock.release( );
        return pmlx_t;
    }
    auto PagingMap::activate( IN pmlxt & pmlxt ) -> VOID {
        lock.acquire( );
        if ( pmlxt.get_table( ) ) {
            ArchitectureManager< TARGET_ARCH >::set_page_table( (uint64_t *)virtual_to_physical( pmlxt.get_table( ) ) );
        }
        lock.release( );
    }
    auto PagingMap::copy( OUT pmlxt & to, IN pmlxt & from ) -> VOID {
        auto page_table = (uint64_t *)physical_to_virtual( Memory::page->allocate( 1, MemoryPageType::PAGE_4K ) );
        memcpy( page_table + 256, from.get_table( ) + 256, PT_SIZE / 2 );
        to = (uint64_t)page_table;
    }
}