#include <Kernel/memory/paging/page_table.hpp>
#include <Lib/spin_lock.hpp>
#include <Kernel/print.hpp>
#include <Arch/Arch.hpp>
PUBLIC namespace QuantumNEC::Kernel {
    inline static Lib::SpinLock lock { };
    inline static __page_allocater allocater { };
    inline static uint64_t address { };
    using namespace std;
    auto __page_table::__VTP_address_from( IN VOID * virtual_address, IN MemoryPageType mode, IN pmlxt & pmlx_t ) -> VOID * {
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
        auto level = __paging::support_5level_paging ? 5 : 4;
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
    auto __page_table::__page_protect( IN BOOL flags ) -> VOID {
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
    auto __page_table::__make( IN uint64_t flags, IN Level level, IN MemoryPageType mode, IN pmlxt & pmlx_t ) -> pmlxt & {
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
        pmlx_t = (uint64_t)allocater.__allocate< MemoryPageType::PAGE_4K >( 1 );
        auto later_flags = flags | pmlx_t.is_huge( mode );
        auto page_size = pmlx_t.check_page_size( mode );
        auto make_helper = [ & ]( this auto &self, IN pmlxt &pmlx_t ) -> VOID {
            if ( ( tlevel - offset ) - 1 ) {
                // 只要没到底，那么就给一块2M内存，存512个4k页表
                taddress = (uint64_t)allocater.__allocate< MemoryPageType::PAGE_2M >( 1 );
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
    auto __page_table::__activate( IN pmlxt & pmlxt ) -> VOID {
        lock.acquire( );
        if ( pmlxt.get_table( ) ) {
            Architecture::ArchitectureManager< TARGET_ARCH >::set_page_table( (uint64_t *)virtual_to_physical( pmlxt.get_table( ) ) );
        }
        lock.release( );
    }
    auto __page_table::__copy( OUT pmlxt & to, IN pmlxt & from ) -> VOID {
        // auto page_table = (uint64_t *)physical_to_virtual( Memory::page->allocate( 1, MemoryPageType::PAGE_4K ) );
        // memcpy( page_table + 256, from.get_table( ) + 256, PT_SIZE / 2 );
        // to = (uint64_t)page_table;
    }
}