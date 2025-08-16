#include <kernel/driver/serial_port/serial_port.hpp>
#include <kernel/memory/allocator/page.hpp>
#include <lib/string.hpp>
namespace Memory::Page {

auto page_memory_initialize( limine_memmap_response *map ) -> void {
    static allocator< Type::P2Mib >::zone zones[ 128 ] { };
    for ( std::uint64_t base = 0; auto &zone : zones ) {
        std::construct_at( &zone, base, Library::RBTree< std::uint64_t, allocator< Type::P2Mib >::zone * >::Node { base, &zone } );
        zone.owner = &zones[ 0 ];
        base += allocator< Type::P2Mib >::__page_size__ * allocator< Type::P2Mib >::page_descriptor_count;
    }
    zones[ 0 ].free_page  = allocator< Type::P2Mib >::page_descriptor_count * 128;
    zones[ 0 ].owner      = nullptr;
    zones[ 0 ].zone_count = 128;

    allocator< Type::P1Gib >::global_memory_mark = 128 * allocator< Type::P2Mib >::__page_size__ * allocator< Type::P2Mib >::page_descriptor_count;

    std::construct_at( &allocator< Type::P2Mib >::zone_tree );

    for ( auto &zone : zones ) {
        allocator< Type::P2Mib >::zone_tree.insert( zone.node );
    }

    for ( auto i = 0ul; i < map->entry_count; ++i ) {
        auto entry = map->entries[ i ];

        allocator< Type::P2Mib >::all_memory_total = entry->base + entry->length;
        switch ( entry->type ) {
        case LIMINE_MEMMAP_USABLE:                     // 未使用
        case LIMINE_MEMMAP_ACPI_RECLAIMABLE:           // ACPI可回收内存
        case LIMINE_MEMMAP_BOOTLOADER_RECLAIMABLE:     // 启动时服务可回收内存
        {
            // 如果是这三种类型那么就计算空闲内存
            auto start_index = ( ( ( entry->base ) + ( allocator< Type::P2Mib >::__page_size__ - 1 ) ) / allocator< Type::P2Mib >::__page_size__ );
            auto end_index   = ( entry->base + entry->length ) / allocator< Type::P2Mib >::__page_size__;
            // 统计空闲内存
            if ( end_index >= start_index ) {
                auto size = end_index - start_index;
                allocator< Type::P2Mib >::free_memory_total += size * allocator< Type::P2Mib >::__page_size__;
            }
        } break;
        case LIMINE_MEMMAP_RESERVED:               // 保留内存
        case LIMINE_MEMMAP_ACPI_NVS:               // ACPI NVS内存
        case LIMINE_MEMMAP_BAD_MEMORY:             // 错误内存
        case LIMINE_MEMMAP_KERNEL_AND_MODULES:     // 模块文件内存
        case LIMINE_MEMMAP_FRAMEBUFFER:            // 显存占用的
            // 计算取得所在区域的header的编号
            auto base_index = ( entry->base & allocator< Type::P2Mib >::__zone_memory_mask__( ) ) / allocator< Type::P2Mib >::__zone_min_memory__;
            // 取得处于所在header的bitmap中的编号
            auto index       = ( entry->base & allocator< Type::P2Mib >::__page_mask__ ) / allocator< Type::P2Mib >::__page_size__ % allocator< Type::P2Mib >::page_descriptor_count;
            auto start_index = entry->base / allocator< Type::P2Mib >::__page_size__;
            auto end_index   = ( ( ( entry->base + entry->length ) + ( allocator< Type::P2Mib >::__page_size__ - 1 ) ) / allocator< Type::P2Mib >::__page_size__ );

            // mark这部分
            zones[ base_index ].pages.set( index, end_index - start_index );
            char buf[ 114 ];
            Library::utoa( ( base_index ), buf, 10 );
            Driver::SerialPort { }.print( buf );
            Driver::SerialPort { }.print( "-" );
            Library::utoa( ( index ), buf, 10 );
            Driver::SerialPort { }.print( buf );
            Driver::SerialPort { }.print( "-" );
            Library::utoa( ( end_index - start_index ), buf, 16 );
            Driver::SerialPort { }.print( buf );
            Driver::SerialPort { }.print( "-" );
            Library::utoa( ( entry->base ), buf, 16 );
            Driver::SerialPort { }.print( buf );
            Driver::SerialPort { }.print( "-" );
            Library::utoa( ( entry->base + entry->length ), buf, 16 );
            Driver::SerialPort { }.print( buf );
            Driver::SerialPort { }.print( "\n" );

            break;
        }
    }
}
}     // namespace Memory::Page
