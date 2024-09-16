#include <Kernel/memory/memory.hpp>
#include <Kernel/print.hpp>
#include <Libcxx/string.hpp>
#include <Lib/deflib.hpp>
#include <Arch/Arch.hpp>
#include <Lib/spin_lock.hpp>
PUBLIC namespace {
    using namespace QuantumNEC;
    using namespace QuantumNEC::Kernel;
    using namespace QuantumNEC::Lib;
    PRIVATE SpinLock lock { };
}

PUBLIC namespace QuantumNEC::Kernel {
    Page::Page( VOID ) noexcept {     // 计算可用内存数量

        Ptr< limine_memmap_response > memory_descriptor { &Architecture::__config.memory_map };
        Bitmap::BitmapEntry bits[ MEMORY_PAGE_DESCRIPTOR ];
        Bitmap bitmap { MEMORY_PAGE_DESCRIPTOR, bits };

        for ( auto i { 0ull }; i < Architecture::__config.memory_map.entry_count; ++i ) {
            auto entry = memory_descriptor->entries[ i ];
            auto start { entry->base }, end { start + entry->length };
            this->all_memory_total = end;     // 这个是总内存
            // 可以使用的内存，可回收内存皆属于空闲内存
            if ( entry->type == LIMINE_MEMMAP_USABLE || entry->type == LIMINE_MEMMAP_ACPI_RECLAIMABLE || entry->type == LIMINE_MEMMAP_BOOTLOADER_RECLAIMABLE ) {
                // 计算内存页边界
                start = DIV_ROUND_UP( start, PAGE_2M_SIZE );
                end = end / PAGE_2M_SIZE;
                // 统计空闲内存
                if ( end >= start ) {
                    this->free_memory_total += ( end - start ) * PAGE_2M_SIZE;
                }
            }
            else {
                // 计算内存页边界
                start = start / PAGE_2M_SIZE;
                end = DIV_ROUND_UP( end, PAGE_2M_SIZE );
                // 将这部分内存添加至bitmap
                for ( auto j { start }; j < end; ++j ) {
                    bitmap[ j ] = { j, 1 };
                }
            }
        }
        for ( auto i { 0ul }; i < 100_MB / PAGE_2M_SIZE; ++i ) {
            // 前100MB内存为保留
            bitmap[ i ] = { i, 1 };
        }
        this->mpg2m.first_init( bitmap );
        this->mpg4k.malloc( 1 );     // 为了给POS腾出空间
        println< ostream::HeadLevel::SYSTEM >( "OS Can Use Memory : {}MB", this->free_memory_total / 1_MB );
    }
    auto Page::malloc( IN size_t size, IN MemoryPageType type ) -> VOID * {
        lock.acquire( );
        using enum MemoryPageType;
        VOID *buffer { };
        switch ( type ) {
        case PAGE_1G:
            buffer = this->mpg1g.malloc( size );
            break;
        case PAGE_2M:
            buffer = this->mpg2m.malloc( size );
            break;
        case PAGE_4K:
            buffer = this->mpg4k.malloc( size );
            break;
        }
        lock.release( );
        return buffer;
    }
    auto Page::free( IN VOID * address, IN size_t size, IN MemoryPageType type ) -> VOID {
        lock.acquire( );
        using enum MemoryPageType;
        switch ( type ) {
        case PAGE_1G:
            this->mpg1g.free( address, size );
            break;
        case PAGE_2M:
            this->mpg2m.free( address, size );
            break;
        case PAGE_4K:
            this->mpg4k.free( address, size );
            break;
        }
        lock.release( );
    }
}