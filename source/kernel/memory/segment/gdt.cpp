#include <kernel/driver/serial_port/serial_port.hpp>
#include <kernel/memory/segment/gdt.hpp>
#include <lib/string.hpp>
#include <memory>
using namespace Memory;
auto GDT::initialize( std::uint64_t core ) -> void {
    using SAccess = SegmentDescriptor::Access;
    using SSAccess = SystemSegmentDescriptor::Access;
    using enum SystemSegmentDescriptor::Access::Type;

    auto &desrciptor = segment_descriptors[ core ];
    std::construct_at( &gdtrs[ core ], desrciptor );
    std::memset( desrciptor, 0, sizeof desrciptor );

    std::construct_at( &desrciptor[ 0 ] );
    std::construct_at( &desrciptor[ 1 ], 0, 0xFFFFF, SAccess { 0, 1, 0, 1, 1, 0, 1 }, ( 1ul << 1 ) | ( 1ul << 3 ) );
    std::construct_at( &desrciptor[ 2 ], 0, 0xFFFFF, SAccess { 0, 1, 0, 0, 1, 0, 1 }, ( 1ul << 2 ) | ( 1ul << 3 ) );
    std::construct_at( &desrciptor[ 3 ], 0, 0xFFFFF, SAccess { 0, 1, 0, 1, 1, 3, 1 }, ( 1ul << 1 ) | ( 1ul << 3 ) );
    std::construct_at( &desrciptor[ 4 ], 0, 0xFFFFF, SAccess { 0, 1, 0, 1, 1, 3, 1 }, ( 1ul << 2 ) | ( 1ul << 3 ) );
    std::construct_at( reinterpret_cast< SystemSegmentDescriptor * >( &desrciptor[ 5 ] ),
                       reinterpret_cast< std::uint64_t >( &tss[ core ] ),
                       sizeof( TaskStateSegment ) - 1,
                       SSAccess { TSS64_AVAILABLE, 0, 0, 1 },
                       0 );

    tss[ core ].io_map_base_address = sizeof( TaskStateSegment ) << 16;

    gdtrs[ core ].write( );
    tss[ core ].load_tr( );
}