#include "Kernel/memory/allocater/page/page_allocater.hpp"
#include "Kernel/memory/collector/page/page_collector.hpp"
#include <Kernel/memory/collector/heap/heap_collector.hpp>
#include <Kernel/memory/manager/heap/heap_manager.hpp>
#include <Lib/spin_lock.hpp>
#include <Libcxx/cstring.hpp>
PUBLIC namespace QuantumNEC::Kernel {
    inline static Lib::SpinLock lock { };
    auto __heap_collector::__free( IN VOID * address ) -> VOID {
        lock.acquire( );
        if ( !address ) {
            lock.release( );
            return;
        }
        // 获取block和zone
        auto *block { reinterpret_cast< __heap_manager::__block * >( reinterpret_cast< uint64_t >( address ) - sizeof( __heap_manager::__block ) ) };
        auto *zone { block->owner };
        // 检测是否为正规的地址
        if ( block->block_magic != __heap_manager::memory_block_magic_size ) {
            lock.release( );
            return;
        }
        // 清空这块内存并设置为空闲
        std::memset( block->start_address, 0, block->size );
        block->is_free = TRUE;
        zone->block_count--;
        if ( !zone->block_count && __heap_manager::global_memory_zone_table.length( ) > 1 ) {
            // 如果整个内存块空闲值为0，那么直接清除整个内存块并从链表中剔除
            // 但是前提是它不是最后一个块
            // 如果是最后一个块那么也是啥也不干，因为要保证一定有一块内存
            __heap_manager::global_memory_zone_table.remove( zone->zone_node );
            __page_collector { }.__free< MemoryPageType::PAGE_2M >( zone, zone->size / __page_allocater::__page_size< MemoryPageType::PAGE_2M > );
            std::memset( zone, 0, zone->size );
        }
        // 反之，啥也不干
        lock.release( );
    }
}