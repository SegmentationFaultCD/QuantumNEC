#include <Kernel/memory/paging/mapper.hpp>
#include <Lib/spin_lock.hpp>
PUBLIC namespace QuantumNEC::Kernel {
    inline static Lib::SpinLock lock { };
    inline static __page_allocater allocater { };
    auto __mapper::__map( IN uint64_t physics_address, IN uint64_t virtual_address, IN uint64_t size, IN uint64_t flags, IN MemoryPageType mode, IN pmlxt & _pmlxt ) -> VOID {
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
        auto _level = __paging::support_5level_paging ? 5 : 4;
        auto get_table = [ & ]( IN uint64_t level ) -> pmlxt & {
            return *page_table[ level - 1 ];
        };
        auto page_size = pml4t.check_page_size( mode );     // 确认每页大小
        flags |= pml4t.is_huge( mode );                     // 如果为huge页那么设置ps位为1

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
                auto new_ = allocater.__allocate< MemoryPageType::PAGE_4K >( 1 );
                pmlx_t = { index, ( reinterpret_cast< uint64_t >( new_ ) & ~0x7FF ), flags };
                std::memset( physical_to_virtual( new_ ), 0, page_size );
            }

            get_table( level - 1 ) = (uint64_t)physical_to_virtual( pmlx_t.flags_base( index ) );     // 得到下一级页表的地址
            self( level - 1, get_table( level - 1 ) );
        };
        while ( size-- ) {     // 重复循环映射
            map_helper( _level, _pmlxt );
        }
        lock.release( );
    }
    auto __mapper::__unmap( IN uint64_t virtual_address, IN size_t size, IN MemoryPageType mode, IN pmlxt & _pmlxt ) -> VOID {
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
        auto _level = __paging::support_5level_paging ? 5 : 4;
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
}