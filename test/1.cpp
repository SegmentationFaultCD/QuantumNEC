#include <cstdint>
#include <numeric>
#include <type_traits>
#include <utility>

struct [[gnu::packed]] Descriptor {
    std::uint16_t offset_low;
    std::uint16_t segment_selector;
    std::uint8_t  ist : 3;
    std::uint8_t : 5;
    struct Attribute {
        enum class GateType : std::uint8_t {
            INTERRUPT = 0xE,
            TRAP      = 0xF
        } gate_type : 4;
        std::uint8_t : 1;
        std::uint8_t DPL : 2;
        std::uint8_t P : 1;
        Attribute( ) = default;
        Attribute( GateType type, std::uint8_t dpl, std::uint8_t p ) :
            gate_type { type },
            DPL { dpl }, P { p } {
        }
    } attribute;
    std::uint16_t offset_middle;
    std::uint32_t offset_high;
    std::uint32_t : 32;
    Descriptor( ) = default;
    Descriptor( uint64_t entry_point, uint16_t selector, uint8_t ist_, Attribute attributes ) :
        offset_low { static_cast< std::uint16_t >( entry_point & 0xffff ) },
        segment_selector { selector },
        ist { static_cast< std::uint8_t >( ist_ & 0b00000111 ) },
        attribute { attributes },
        offset_middle { static_cast< std::uint16_t >( ( entry_point >> 16 ) & 0xffff ) },
        offset_high { static_cast< std::uint32_t >( ( entry_point >> 32 ) & 0xffffffff ) } {
    }
};
struct [[gnu::packed]] SegmentDescriptor {
    std::uint16_t limit_low;
    std::uint16_t base_low;
    std::uint8_t  base_middle;
    struct [[gnu::packed]] Access {
        std::uint8_t A : 1;
        std::uint8_t RW : 1;
        std::uint8_t DC : 1;
        std::uint8_t E : 1;
        std::uint8_t S : 1;
        std::uint8_t DPL : 2;
        std::uint8_t P : 1;
    } access_right;
    std::uint8_t limit_high : 4;

    std::uint8_t flags : 4;     // clang的奇葩地方结构体里面位域不写满就算1字节只能用uint8了（恼
    std::uint8_t base_high;
    SegmentDescriptor( void ) {
    }
    SegmentDescriptor( std::uint64_t base, std::uint64_t limit, Access access, std::uint8_t flag ) :
        limit_low { static_cast< std::uint16_t >( limit & 0xffff ) },
        base_low { static_cast< std::uint16_t >( base & 0xffff ) },
        base_middle { static_cast< std::uint8_t >( ( base & 0xff0000 ) >> 16 ) },
        access_right { access },
        limit_high { static_cast< std::uint8_t >( ( ( limit >> 16 ) & 0xf ) | ( ( std::bit_cast< std::uint8_t >( access ) >> 8 ) & 0xf0 ) ) },
        flags { static_cast< std::uint8_t >( flag & 0xf ) },
        base_high { static_cast< std::uint8_t >( ( base >> 24 ) & 0xff ) } {
    }
};
struct [[gnu::packed]] SystemSegmentDescriptor {
    std::uint16_t limit_low;
    std::uint16_t base_low;
    std::uint8_t  base_middle;
    struct [[gnu::packed]] {
        enum class Type : std::uint8_t {
            LDT             = 0x2,
            TSS64_AVAILABLE = 0x9,
            TSS64_BUSY      = 0xB
        };
        Type         type : 4;
        std::uint8_t S : 1;
        std::uint8_t DPL : 2;
        std::uint8_t P : 1;
    } access_right;
    std::uint8_t  limit_high : 4;
    std::uint8_t  flags : 4;
    std::uint64_t base_high : 40;
    std::uint32_t : 32;
};
#include <bit>
#include <print>
auto main( void ) -> int {
    SegmentDescriptor::Access a { 0, 1, 0, 1, 1, 0, 1 };

    std::println( "{:x}", std::bit_cast< std::uint8_t >( a ) );
}