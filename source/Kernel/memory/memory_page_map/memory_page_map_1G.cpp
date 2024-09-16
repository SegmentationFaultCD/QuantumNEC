#include <Kernel/memory/memory_page_map/memory_map_1G.hpp>
#include <Kernel/memory/pmlxt/pmlxt.hpp>
#include <Libcxx/string.hpp>
#include <Lib/spin_lock.hpp>
PUBLIC namespace {
    using namespace QuantumNEC::Lib;
    PRIVATE SpinLock lock { };
}
PUBLIC namespace QuantumNEC::Kernel {
    auto MemoryPageMap1G::malloc( IN uint64_t size )->VOID * {
        lock.acquire( );
        if ( !size ) {
            lock.release( );
            // size空的就没有分配
            return NULL;
        }
        PageHandler *handler { };
        MemoryPageMap1G mpm1g { };

        if ( size >= MEMORY_PAGE_DESCRIPTOR ) {     // 这直接分配一块 PAGE_1G_SIZE * MEMORY_PAGE_DESCRIPTOR内存了
            // 首先计算handler的数量，如果handler的数量恰好可以除得尽那么就是size / MEMORY_PAGE_DESCRIPTOR，如果除不尽那就向上取整

            auto handler_count = ( size % MEMORY_PAGE_DESCRIPTOR ) ? size / MEMORY_PAGE_DESCRIPTOR : DIV_ROUND_UP( size, MEMORY_PAGE_DESCRIPTOR );
            auto flag = ( size % MEMORY_PAGE_DESCRIPTOR ) == 0;
            auto handler_block_count { 1ul };     // 头信息块数量
            auto bitmap_block_count { 1ul };      // bitmap块数量
            for ( ; ( PAGE_1G_SIZE * handler_block_count ) / ( handler_count * sizeof( PageHandler ) ) > 0; ++handler_block_count )
                ;     // 计算要多少个2m页来存放头信息块
            for ( ; ( PAGE_1G_SIZE * bitmap_block_count ) / ( handler_count * sizeof( PageBlock ) ) > 0; ++bitmap_block_count )
                ;     // 计算要多少个2m页来存放bitmap块

            PageBlock *bitmap_block { (PageBlock *)info_allocater.PAGE_BITMAP_VIRTUAL_BASE };
            PageHandler *handler_block { (PageHandler *)info_allocater.PAGE_HANDLER_INFORMATION_VIRTUAL_BASE };
            if ( info_allocater.offset_bitmap( this ) < bitmap_block_count ) {
                info_allocater.allocate_bitmap( bitmap_block_count, this );
            }
            if ( info_allocater.offset_handler( this ) < handler_block_count ) {
                info_allocater.allocate_handler( handler_block_count, this );
            }

            if ( !flag ) {     // 除的尽
                // 全部填充

                while ( handler_count ) {
                    // 计算存储头信息所需要的空间大小
                    handler = new ( physical_to_virtual( &info_allocater.add( handler_block ) ) ) PageHandler { info_allocater.add( bitmap_block ) };
                    handler->node.container = handler;
                    memory_page_table_1G.push( handler->node );           // 将节点插入全局队列
                    handler->map_base_adderess = this->global_memory;     // 确定base地址，用于free时借助算法节约时间
                    this->global_memory += MEMORY_PAGE_DESCRIPTOR * PAGE_1G_SIZE;
                    for ( auto i { 0ul }; i < MEMORY_PAGE_DESCRIPTOR; ++i ) {
                        handler->bitmap[ i ] = { i, BITMAP_MASK };
                    }
                    handler->flags.set_full( PAGE_FLAGS_FULL );
                    memset( physical_to_virtual( handler->map_base_adderess ), 0, PAGE_1G_SIZE * MEMORY_PAGE_DESCRIPTOR );
                    handler_count--;
                }
            }
            else {
                // 头块，记录
                auto _handler = new ( physical_to_virtual( &info_allocater.add( handler_block ) ) ) PageHandler { info_allocater.add( bitmap_block ) };
                _handler->node.container = _handler;
                memory_page_table_1G.push( _handler->node );           // 将节点插入全局队列
                _handler->map_base_adderess = this->global_memory;     // 确定base地址，用于free时借助算法节约时间
                this->global_memory += PAGE_1G_SIZE * MEMORY_PAGE_DESCRIPTOR;
                handler = _handler;
                for ( auto i { 0ul }; i < MEMORY_PAGE_DESCRIPTOR; ++i ) {
                    _handler->bitmap[ i ] = { i, BITMAP_MASK };
                }
                _handler->flags.set_full( PAGE_FLAGS_FULL );
                memset( physical_to_virtual( _handler->map_base_adderess ), 0, PAGE_1G_SIZE * MEMORY_PAGE_DESCRIPTOR );
                // 头块后面的所有块，但不包括最后一块
                for ( auto c { 1ul }; c < handler_count - 1; ++c ) {
                    _handler = new ( physical_to_virtual( &info_allocater.add( handler_block ) ) ) PageHandler { info_allocater.add( bitmap_block ) };
                    _handler->node.container = _handler;
                    memory_page_table_1G.push( _handler->node );           // 将节点插入全局队列
                    _handler->map_base_adderess = this->global_memory;     // 确定base地址，用于free时借助算法节约时间
                    this->global_memory += PAGE_1G_SIZE * MEMORY_PAGE_DESCRIPTOR;
                    for ( auto i { 0ul }; i < MEMORY_PAGE_DESCRIPTOR; ++i ) {
                        _handler->bitmap[ i ] = { i, BITMAP_MASK };
                    }
                    _handler->flags.set_full( PAGE_FLAGS_FULL );
                    memset( physical_to_virtual( _handler->map_base_adderess ), 0, PAGE_1G_SIZE * MEMORY_PAGE_DESCRIPTOR );
                }
                // 最后一块
                _handler = new ( physical_to_virtual( &info_allocater.add( handler_block ) ) ) PageHandler { info_allocater.add( bitmap_block ) };
                _handler->node.container = _handler;
                memory_page_table_1G.push( _handler->node );           // 将节点插入全局队列
                _handler->map_base_adderess = this->global_memory;     // 确定base地址，用于free时借助算法节约时间
                this->global_memory += PAGE_1G_SIZE * MEMORY_PAGE_DESCRIPTOR;
                for ( auto i { 0ul }; i < ( size % MEMORY_PAGE_DESCRIPTOR ); ++i ) {
                    _handler->bitmap[ i ] = { i, BITMAP_MASK };
                }
                memset( physical_to_virtual( _handler->map_base_adderess ), 0, PAGE_1G_SIZE * ( size % MEMORY_PAGE_DESCRIPTOR ) );
            }
            lock.release( );
            return (VOID *)handler->map_base_adderess;
        }
        else if ( size < MEMORY_PAGE_DESCRIPTOR ) {
            if ( !memory_page_table_1G.is_empty( ) ) {
                handler = ( (PageHandler *)memory_page_table_1G.pop( )->container );
                memory_page_table_1G.push( handler->node );
                if ( handler->flags.flag_full( ) )     // 满了
                {
                    info_allocater.allocate_bitmap( 1, this );
                    info_allocater.allocate_handler( 1, this );
                    // 新建一块内存页信息块

                    handler = new ( physical_to_virtual( &info_allocater.add( (PageHandler *)info_allocater.PAGE_HANDLER_INFORMATION_VIRTUAL_BASE ) ) ) PageHandler { info_allocater.add( (PageBlock *)info_allocater.PAGE_BITMAP_VIRTUAL_BASE ) };
                    handler->node.container = handler;
                    memory_page_table_1G.push( handler->node );     // 将节点插入全局队列
                    this->global_memory += MEMORY_PAGE_DESCRIPTOR * PAGE_1G_SIZE;
                }
                // 没有满，那么就用这个原来的块
            }

            auto status { Architecture::ArchitectureManager< TARGET_ARCH >::disable_interrupt( ) };
            auto index = handler->bitmap.allocate( size );
            VOID *buffer { };
            if ( index != -1 ) {
                for ( auto i { 0ul }; i < index + size; ++i ) {
                    handler->bitmap[ i ] = { i, 1 };
                }
                buffer = (VOID *)( handler->map_base_adderess + index * PAGE_1G_SIZE );
                memset( physical_to_virtual( buffer ), 0, size * PAGE_2M_SIZE );
            }

            if ( handler->bitmap.is_full( ) ) {
                // 如果满了，那就要把标志位设置一个1
                handler->flags.set_flags( PAGE_FLAGS_FULL );
            }
            Architecture::ArchitectureManager< TARGET_ARCH >::set_interrupt( status );
            lock.release( );
            return buffer;
        }
        return NULL;
    }
    auto MemoryPageMap1G::free( IN VOID * address, IN uint64_t size )->VOID {
        lock.acquire( );
        if ( !size || !address || size > MEMORY_PAGE_DESCRIPTOR ) {
            lock.release( );
            return;
        }
        auto base = (uint64_t)address & ( ~( PAGE_1G_SIZE * MEMORY_PAGE_DESCRIPTOR ) );
        auto node = memory_page_table_1G.traversal(
            [ & ]( ListNode *node, uint64_t ) -> BOOL {
                return ( (PageHandler *)node->container )->map_base_adderess == base;
            },
            0 );
        if ( !node ) {
            lock.release( );
            return;
        }
        auto index = ( (uint64_t)address - base ) / PAGE_1G_SIZE;
        auto handler = (PageHandler *)node->container;
        auto status { Architecture::ArchitectureManager< TARGET_ARCH >::disable_interrupt( ) };
        for ( auto i { index }; i < index + size; ++i ) {
            handler->bitmap[ i ] = { i, 0 };
        }
        // 清空之前废弃的数据
        memset( physical_to_virtual( address ), 0, size * PAGE_1G_SIZE );
        Architecture::ArchitectureManager< TARGET_ARCH >::set_interrupt( status );
        lock.release( );
    }
}