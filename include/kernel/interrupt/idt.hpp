#pragma once
#include <kernel/driver/cpu/register.hpp>
#include <kernel/utils/descriptor_register.hpp>
namespace Interrupt {
class IDT {
public:
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
    struct [[gnu::packed]] Frame {
        Driver::RegisterFrame regs;
        uint64_t              vector;
        uint64_t              error_code;
        void                 *rip;
        uint64_t              cs;
        Driver::RFlags        rflags;
        uint64_t              rsp;
        uint64_t              ss;
        explicit Frame( void ) noexcept = default;
    };

private:
    IDT( void ) = delete;

public:
    static auto initialize( std::uint64_t core ) -> void;

private:
    class [[gnu::packed]] DescriptorRegister final : public Utils::DescriptorRegister< Descriptor, 256 > {
    public:
        DescriptorRegister( void ) :
            Utils::DescriptorRegister< Descriptor, 256 > { nullptr } {
        }
        DescriptorRegister( Descriptor *_offset ) :
            Utils::DescriptorRegister< Descriptor, 256 > { _offset } {
        }

    public:
        auto read_( void ) -> Descriptor * {
            __asm__ __volatile__( "sidt %0" : "=m"( *this ):: );
            return this->offset;
        }
        auto write_( void ) -> void {
            __asm__ __volatile__( "lidt %0" ::"m"( *this ) : );
        }
    };

    inline static Descriptor         interrupt_descriptors[ 256 ] { };
    inline static DescriptorRegister idtr { nullptr };
};
}     // namespace Interrupt