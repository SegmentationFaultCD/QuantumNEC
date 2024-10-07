#include <Kernel/memory/allocater/heap/heap_allocater.hpp>
#include <Kernel/memory/allocater/page/page_allocater.hpp>
#include <Kernel/memory/manager/heap/heap_manager.hpp>
#include <Lib/spin_lock.hpp>
PUBLIC namespace QuantumNEC::Kernel {
    inline static Lib::SpinLock lock { };
    inline static __page_allocater allocater { };
    auto __heap_allocater::__allocate( IN uint64_t size ) -> VOID * {
        lock.acquire( );
        if ( size <= 0 ) {
            lock.release( );
            return NULL;
        }
        __heap_manager::__zone *zone { };
        __heap_manager::__block *block { };
        if ( size < __page_allocater::__page_size< MemoryPageType::PAGE_2M > ) {
            // 不超过一页大小的就在页内分配
            zone = reinterpret_cast< decltype( zone ) >( __heap_manager::global_memory_zone_table.traversal( [ & ]( Lib::ListNode *node, uint32_t ) -> BOOL { return !reinterpret_cast< decltype( zone ) >( node->container )->is_full; }, 0 )->container );

            if ( zone->block_table.is_empty( ) ) {
                // 如果整个块都为空闲状态，那么就直接在zone结构后面作为起始地址开始分配
                // 也就是从这个内存区域开头开始分配
                block = reinterpret_cast< decltype( block ) >( zone->zone_start_address );
            }
            else {
                // 如果这块内存区域不为空，那么就查找是否有之前分配的符合条件的内存块
                auto *node { zone->block_table.traversal(
                    [ & ]( Lib::ListNode *node, uint32_t ) -> BOOL { return reinterpret_cast< decltype( block ) >( node->container )->is_free && ( reinterpret_cast< decltype( block ) >( node->container )->size == size ); }, 0 ) };
                if ( !node ) {
                    // 如果这个块中没有以前分配的符合条件的内存块，那么就开辟新的内存块

                    block = reinterpret_cast< decltype( block ) >( reinterpret_cast< uint64_t >( reinterpret_cast< decltype( block ) >( zone->block_table.end.prev->container )->end_address ) );
                    if ( reinterpret_cast< uint64_t >( block ) + size > reinterpret_cast< uint64_t >( zone->zone_end_address ) ) {
                        // 如果剩余内存区域空间不足以分配一个新内存块
                        if ( auto last_free_memory { reinterpret_cast< uint64_t >( zone->zone_end_address ) - reinterpret_cast< uint64_t >( block ) }; last_free_memory > sizeof *block ) {
                            // 如果剩下的空间支持分配1字节及以上的内存+block块大小那么就将它整理
                            block->owner = zone;
                            block->block_node.container = reinterpret_cast< decltype( block->block_node.container ) >( block );
                            zone->block_table.append( block->block_node );
                            block->start_address = reinterpret_cast< decltype( block->start_address ) >( reinterpret_cast< uint64_t >( block ) + sizeof *block );
                            block->end_address = reinterpret_cast< decltype( block->end_address ) >( reinterpret_cast< uint64_t >( block->start_address ) + last_free_memory );
                            block->size = last_free_memory;
                            block->is_free = FALSE;
                            block->block_magic = __heap_manager::memory_block_magic_size;
                        }
                        // 如果连block都不足已分配那么直接放弃这块内存，设置整个内存块已满
                        zone->is_full = TRUE;
                        // 并开辟新内存区域
                        zone = new ( physical_to_virtual( allocater.__allocate< MemoryPageType::PAGE_2M >( 1 ) ) ) __heap_manager::__zone { };
                        zone->zone_node.container = reinterpret_cast< decltype( zone->zone_node.container ) >( zone );
                        zone->is_full = FALSE;
                        zone->size = __page_allocater::__page_size< MemoryPageType::PAGE_2M >;
                        zone->zone_start_address = reinterpret_cast< decltype( zone->zone_start_address ) >( reinterpret_cast< uint64_t >( zone ) + sizeof *zone );
                        zone->zone_end_address = reinterpret_cast< decltype( zone->zone_end_address ) >( reinterpret_cast< uint64_t >( zone ) + zone->size );
                        __heap_manager::global_memory_zone_table.append( zone->zone_node );
                        block = reinterpret_cast< decltype( block ) >( zone->zone_start_address );
                    }
                }
                else {
                    block = reinterpret_cast< decltype( block ) >( node->container );
                    // 有，那么就设置标志位为FALSE并返回这个内存块指向的地址
                    block->is_free = FALSE;
                    zone->block_count++;
                    std::memset( block->start_address, 0, size );
                    lock.release( );
                    return block->start_address;
                }
            }
        }
        else if ( size >= __page_allocater::__page_size< MemoryPageType::PAGE_2M > ) {
            auto page_count { size % __page_allocater::__page_size< MemoryPageType::PAGE_2M > ? size / __page_allocater::__page_size< MemoryPageType::PAGE_2M > + 1 : size / __page_allocater::__page_size< MemoryPageType::PAGE_2M > };
            // 超过一页大小的分配量那么直接分配多个页或者一个整页
            zone = new ( physical_to_virtual( allocater.__allocate< MemoryPageType::PAGE_2M >( page_count ) ) ) __heap_manager::__zone { };
            zone->zone_node.container = reinterpret_cast< decltype( zone->zone_node.container ) >( zone );
            zone->is_full = size % __page_allocater::__page_size< MemoryPageType::PAGE_2M > ? FALSE : TRUE;
            zone->size = page_count * __page_allocater::__page_size< MemoryPageType::PAGE_2M >;
            zone->zone_start_address = reinterpret_cast< decltype( zone->zone_start_address ) >( reinterpret_cast< uint64_t >( zone ) + sizeof *zone );
            zone->zone_end_address = reinterpret_cast< decltype( zone->zone_end_address ) >( reinterpret_cast< uint64_t >( zone ) + zone->size );
            __heap_manager::global_memory_zone_table.append( zone->zone_node );
            block = reinterpret_cast< decltype( block ) >( zone->zone_start_address );
        }
        // 初始化内存块
        block->block_node.container = block;
        zone->block_table.append( block->block_node );
        block->owner = zone;
        block->start_address = reinterpret_cast< decltype( block->start_address ) >( reinterpret_cast< uint64_t >( block ) + sizeof *block );
        block->end_address = reinterpret_cast< decltype( block->end_address ) >( reinterpret_cast< uint64_t >( block->start_address ) + size );
        block->size = size;
        block->is_free = FALSE;
        block->block_magic = __heap_manager::memory_block_magic_size;
        zone->block_count++;
        std::memset( block->start_address, 0, size );
        lock.release( );
        return block->start_address;
    }
}