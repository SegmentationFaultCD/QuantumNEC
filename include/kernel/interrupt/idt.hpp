#pragma once
#include <kernel/driver/cpu/register.hpp>
#include <kernel/utils/descriptor_register.hpp>
namespace Interrupt {
class IDT {
    /* ------------------------------------------------------------------------------------
     * 0 ~ 20   异常入口
     * 21 ~ 26  保留
     * 27 ~ 29  异常入口
     * 30 ~ 256 中断入口
        - 30 ~ 31 无用处
        - 32 时间中断
        - 33 键盘中断
        - 34 级联(两个芯片内部使用。不会发起)/HPET timer 0/8254 counter 0
        - 35 串口COM2&4对应的入口
        - 36 串口COM1&3对应的入口
        - 37 并口LPT2对应的入口
        - 38 软盘对应的入口
        - 39 并口LPT1对应的入口 [通常是“伪”中断（不可靠）]
        - 40 CMOS实时时钟/HPET timer 1
        - 41 自由外设/legacy SCSI/网卡
        - 42 自由外设/SCSI/网卡
        - 43 HPET timer 2/自由外设/SCSI/网卡
        - 44 HPET timer 3/PS2鼠标接口
        - 45 FPU/协处理器/间处理器
        - 46 主ATA硬盘
        - 47 从ATA硬盘
        - 48 PIRQA
        - 49 PIRQB
        - 50 PIRQC
        - 51 PIRQD
        - 52 PIRQE
        - 53 PIRQF
        - 54 PIRQG
        - 55 PIRQH
        - 128 系统调用
        - 150 ~ 200	Local APIC
        - 150	CMCI
        - 151	Timer
        - 152	Thermal Monitor
        - 153	Performance Counter
        - 154	LINT0
        - 155	LINT1
        - 156	Error
        - 157   Spurious
        - 200 ~ 255 SMP

     * ------------------------------------------------------------------------------------ */
public:
    constexpr static auto CLOCK { 32 };
    constexpr static auto APIC_ERROR { 156 };
    constexpr static auto APIC_SPURIOUS { 157 };

public:
    struct [[gnu::packed]] Descriptor {
        std::uint16_t offset_low;
        std::uint16_t segment_selector;
        std::uint8_t ist : 3;
        std::uint8_t : 5;
        struct Attribute {
            enum class GateType : std::uint8_t {
                INTERRUPT = 0xE,
                TRAP = 0xF
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
        uint64_t vector;
        uint64_t error_code;
        void *rip;
        uint64_t cs;
        Driver::RFlags rflags;
        uint64_t rsp;
        uint64_t ss;
        explicit Frame( void ) noexcept = default;
    };

private:
    IDT( void ) = delete;

public:
    static auto initialize( std::uint64_t core ) -> void;
    static auto enable_interrupt( ) {
        __asm__ __volatile__( "sti" );
        __asm__ __volatile__( "std" );
    }
    static auto disable_interrupt( ) {
        __asm__ __volatile__( "cli" );
        __asm__ __volatile__( "cld" );
    }

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

    inline static Descriptor interrupt_descriptors[ 256 ] { };
    inline static DescriptorRegister idtr { nullptr };
};
}     // namespace Interrupt
