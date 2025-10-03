#include <kernel/driver/serial_port/serial_port.hpp>
#include <kernel/memory/segment/gdt.hpp>
#include <lib/string.hpp>
#include <memory>
using namespace Memory;
namespace {
std::uint8_t ist[ 4096 ][ GDT::GDT_COUNT ] { };
}
auto GDT::initialize( std::uint64_t core ) -> GDT * {
    static GDT gdt { };

    auto &desrciptor = gdt.segment_descriptors[ core ];
    std::construct_at( &gdt.gdtrs[ core ], desrciptor );
    std::memset( desrciptor, 0, sizeof desrciptor );

    std::construct_at( &desrciptor[ 0 ] );
    // kernel code segment
    std::construct_at( &desrciptor[ 1 ], 0, 0xFFFFF, 0x9A /* 0x9A  SAccess { 0, 1, 0, 1, 1, 0, 1 }*/, ( 1ul << 1 ) | ( 1ul << 3 ) );
    // kernel data segment
    std::construct_at( &desrciptor[ 2 ], 0, 0xFFFFF, 0x92 /*   SAccess { 0, 1, 0, 0, 1, 0, 1 }*/, ( 1ul << 2 ) | ( 1ul << 3 ) );
    // user data segment
    std::construct_at( &desrciptor[ 3 ], 0, 0xFFFFF, 0xF2 /*  SAccess { 0, 1, 0, 1, 1, 3, 1 }*/, ( 1ul << 2 ) | ( 1ul << 3 ) );
    // user code segment
    std::construct_at( &desrciptor[ 4 ], 0, 0xFFFFF, 0xFA /*   SAccess { 0, 1, 0, 1, 1, 3, 1 }*/, ( 1ul << 1 ) | ( 1ul << 3 ) );

    std::construct_at( reinterpret_cast< SystemSegmentDescriptor * >( &desrciptor[ 5 ] ),
                       reinterpret_cast< std::uint64_t >( &gdt.tss[ core ] ),
                       sizeof( TaskStateSegment ) - 1,
                       0x89,
                       // SystemSegmentDescriptor::Access { TSS64_AVAILABLE, 0, 0, 1 },
                       0 );

    gdt.tss[ core ].io_map_base_address = static_cast< std::uint16_t >( sizeof( TaskStateSegment ) << 16 );
    gdt.tss[ core ].ist[ 0 ] = (uint64_t)ist[ core ] + sizeof( std::uint8_t ) * 4096;

    gdt.gdtrs[ core ].write( );
    gdt.tss[ core ].load_tr( );
    return &gdt;
}