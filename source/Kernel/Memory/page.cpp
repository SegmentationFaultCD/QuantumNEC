#include <Kernel/Memory/map.hpp>
#include <Kernel/Memory/page.hpp>
#include <Lib/IO/Stream/iostream>
#include <Lib/STL/string>
#include <Lib/Base/deflib.hpp>
#include <Arch/Arch.hpp>
#include <Kernel/task.hpp>
PUBLIC namespace {
    PUBLIC using namespace QuantumNEC::Lib::IO;
    PUBLIC using namespace QuantumNEC;
    PUBLIC using namespace QuantumNEC::Kernel;

    PRIVATE Lib::Types::Ptr< VOID > buffer_4k { };
    PRIVATE TaskLock lock { };
}

PUBLIC namespace QuantumNEC::Kernel {
    PageMemory::PageMemory( IN Lib::Types::Ptr< Lib::Types::BootConfig > _config ) noexcept {                        // 计算可用内存数量
        Lib::Types::Ptr< Lib::Types::EfiMemoryDescriptor > efi_memory_descriptor { _config->memory_map.Buffer };     // memory map
        bitmap_.set_length( MEMORY_PAGE_DESCRIPTOR );
        bitmap_.set_bits( page_descriptor_entry );
        auto check_memory_type = [ &, this ]( Lib::Types::size_t n ) -> MemoryPageAttribute {
            switch ( efi_memory_descriptor[ n ].Type ) {
            case Lib::Types::EfiMemoryType::EfiConventionalMemory:
            case Lib::Types::EfiMemoryType::EfiBootServicesCode:
            case Lib::Types::EfiMemoryType::EfiBootServicesData:
            case Lib::Types::EfiMemoryType::EfiLoaderCode:
            case Lib::Types::EfiMemoryType::EfiLoaderData:
                return MemoryPageAttribute::FREE_MEMORY;     // 可用空闲内存
            case Lib::Types::EfiMemoryType::EfiRuntimeServicesCode:
            case Lib::Types::EfiMemoryType::EfiRuntimeServicesData:
            case Lib::Types::EfiMemoryType::EfiMemoryMappedIO:
            case Lib::Types::EfiMemoryType::EfiMemoryMappedIOPortSpace:
            case Lib::Types::EfiMemoryType::EfiPalCode:
            case Lib::Types::EfiMemoryType::EfiReservedMemoryType:
                return MemoryPageAttribute::RESERVED_MEMORY;     // 保留内存
            case Lib::Types::EfiMemoryType::EfiACPIReclaimMemory:
                return MemoryPageAttribute::ACPI_MEMORY;     // ACPI 保留内存
            case Lib::Types::EfiMemoryType::EfiACPIMemoryNVS:
                return MemoryPageAttribute::ACPI_MEMORY_NVS;     // ACPI 可回收内存
            case Lib::Types::EfiMemoryType::EfiUnusableMemory:
            case Lib::Types::EfiMemoryType::EfiMaxMemoryType:
                return MemoryPageAttribute::UNUSEABLE_MEMORY;     // 不可用内存
            default:
                return MemoryPageAttribute::UNDEF_MEMORY;     // 其他未定义内存
            }
        };
        for ( Lib::Types::size_t i { }; i < _config->memory_map.MemoryCount; ++i ) {
            MemoryPageAttribute type { check_memory_type( i ) };
            Lib::Types::uint64_t start { efi_memory_descriptor[ i ].PhysicalStart }, end { start + ( efi_memory_descriptor[ i ].NumberOfPages << 12 ) };
            this->general_memory_total = end;
            if ( type == MemoryPageAttribute::FREE_MEMORY ) {     // 如果为空闲内存
                // 计算内存页边界
                start = Lib::Base::DIV_ROUND_UP( start, PAGE_SIZE );
                end = end / PAGE_SIZE;
                // 统计空闲内存
                if ( end >= start ) {
                    this->memory_total += ( end - start ) * PAGE_SIZE;
                }
            }
            else {
                // 计算内存页边界
                start = start / PAGE_SIZE;
                end = Lib::Base::DIV_ROUND_UP( end, PAGE_SIZE );
                // 将这部分内存添加至bitmap
                for ( Lib::Types::uint64_t j { start }; j < end; ++j ) {
                    this->bitmap_.set( j, 1 );
                }
            }
        }

        if ( ( this->general_memory_total / PAGE_SIZE ) < MEMORY_PAGE_DESCRIPTOR ) {
            this->bitmap_.set_length( this->general_memory_total / PAGE_SIZE );
        }

        sout[ ostream::HeadLevel::SYSTEM ] << "OS Can Use Memory : " << this->memory_total / 1_MB << " MB";
        // 显示可用内存数
        if ( auto GB_memory_total { ( this->memory_total + 28_MB ) / 1_MB / 1_KB }; GB_memory_total ) {
            sout << " <=> " << GB_memory_total << " GB";
        }
        endl( sout );
        buffer_4k = malloc_2M_page( 20 );
    }
    auto PageMemory::malloc( IN Lib::Types::size_t size, IN MemoryPageType type )->Lib::Types::Ptr< VOID > {
        lock.acquire( );
        switch ( type ) {
        case MemoryPageType::PAGE_4K:
            lock.release( );
            return malloc_4K_page( size );
        case MemoryPageType::PAGE_2M:
            lock.release( );
            return malloc_2M_page( size );
        case MemoryPageType::PAGE_1G:
            lock.release( );
            return malloc_1G_page( size );
        default:
            lock.release( );
            return NULL;
        };
    }
    auto PageMemory::free( IN Lib::Types::Ptr< VOID > address, IN Lib::Types::size_t size, IN MemoryPageType type )->VOID {
        lock.acquire( );
        switch ( type ) {
        case MemoryPageType::PAGE_4K:
            free_4K_page( address, size );
            break;
        case MemoryPageType::PAGE_2M:
            free_2M_page( address, size );
            break;
        case MemoryPageType::PAGE_1G:
            free_1G_page( address, size );
            break;
        default:
            break;
        };
        lock.release( );
    }
    auto PageMemory::malloc_2M_page( IN Lib::Types::size_t size )->Lib::Types::Ptr< VOID > {
        Lib::Types::Ptr< VOID > buffer { };
        Architecture::ArchitectureManager< TARGET_ARCH >::InterruptStatus status { Architecture::ArchitectureManager< TARGET_ARCH >::disable_interrupt( ) };
        if ( size ) {
            Lib::Types::int64_t index { bitmap_.allocate( size ) };
            if ( index != -1 ) {
                for ( Lib::Types::uint64_t j { }; j < index + size; ++j ) {
                    bitmap_.set( j, 1 );
                }
                buffer = reinterpret_cast< decltype( buffer ) >( 0ULL + index * PAGE_SIZE );
                Lib::STL::memset( buffer, 0, size * PAGE_SIZE );
            }
        }
        else {
            buffer = NULL;
        }
        Architecture::ArchitectureManager< TARGET_ARCH >::set_interrupt( status );
        return buffer;
    }
    auto PageMemory::free_2M_page( IN Lib::Types::Ptr< VOID > address, IN Lib::Types::size_t size )->VOID {
        if ( !size || !address ) {
            return;
        }
        Architecture::ArchitectureManager< TARGET_ARCH >::InterruptStatus status { Architecture::ArchitectureManager< TARGET_ARCH >::disable_interrupt( ) };
        for ( Lib::Types::uint64_t index { reinterpret_cast< decltype( index ) >( address ) / PAGE_SIZE }; index < reinterpret_cast< decltype( index ) >( address ) / PAGE_SIZE + size; ++index ) {
            bitmap_.set( index, 0 );
        }
        // 清空之前废弃的数据
        Lib::STL::memset( address, 0, size * PAGE_SIZE );
        Architecture::ArchitectureManager< TARGET_ARCH >::set_interrupt( status );
    }
    // typedef struct
    // {
    //     Lib::Types::BOOL is_used;
    //     Lib::Types::uint64_t size;
    //     Lib::Types::Ptr< VOID > buffer_address;
    // } _packed 4K_PAGE;
    // int count { };
    auto PageMemory::malloc_4K_page( IN Lib::Types::size_t size )->Lib::Types::Ptr< VOID > {
        buffer_4k = reinterpret_cast< decltype( buffer_4k ) >( reinterpret_cast< Lib::Types::uint64_t >( buffer_4k ) + size * 4_KB );
        return buffer_4k;
        // for ( Lib::Types::uint64_t i { }; i < 20_MB / ( 4_KB + sizeof( 4K_PAGE ) ); ++i ) {
        //     page = reinterpret_cast< decltype( page ) >( reinterpret_cast< Lib::Types::uint64_t >( buffer ) + i * ( 4_KB + sizeof( 4K_PAGE ) ) );
        //     if ( !page->is_used ) {
        //         page->is_used = TRUE;
        //         return reinterpret_cast< Lib::Types::Ptr< VOID > >( reinterpret_cast< Lib::Types::uint64_t >( page ) + sizeof( 4K_PAGE ) );
        //     }
        // }
        // return NULL;
    }
    auto PageMemory::free_4K_page( IN Lib::Types::Ptr< VOID >, IN Lib::Types::size_t size )->VOID {
        buffer_4k = reinterpret_cast< decltype( buffer_4k ) >( reinterpret_cast< Lib::Types::uint64_t >( buffer_4k ) - size * 4_KB );
    }
    auto PageMemory::malloc_1G_page( IN Lib::Types::size_t )->Lib::Types::Ptr< VOID > {
        return NULL;
    }
    auto PageMemory::free_1G_page( IN Lib::Types::Ptr< VOID >, IN Lib::Types::size_t )->VOID {
    }
}