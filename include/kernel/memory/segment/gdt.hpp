#pragma once
#include <bit>
#include <cstdint>
#include <kernel/utils/descriptor_register.hpp>
#include <utility>
namespace Memory {
class GDT {
public:
    constexpr static auto SELECTOR_CODE64_KERNEL = 0x8 | 0x0;
    constexpr static auto SELECTOR_DATA64_KERNEL = 0x10 | 0x0;
    constexpr static auto SELECTOR_DATA64_USER = 0x18 | 0x3;     // 遵循syscall ABI
    constexpr static auto SELECTOR_CODE64_USER = 0x20 | 0x3;
    constexpr static auto SELECTOR_TSS = 0x28 | 0x0;
    constexpr static auto GDT_COUNT = 256;

public:
    struct [[gnu::packed]] SegmentDescriptor {
        std::uint64_t limit_low : 16;
        std::uint64_t base_low : 24;
        std::uint64_t access_right : 8;
        std::uint64_t limit_high : 4;
        std::uint64_t flags : 4;     // clang的奇葩地方结构体里面位域不写满就算1字节只能用uint8了（恼
        std::uint64_t base_high : 8;
        SegmentDescriptor( void ) {
        }
        SegmentDescriptor( std::uint32_t base, std::uint32_t limit, std::uint8_t access, std::uint8_t flag ) :
            limit_low { static_cast< std::uint16_t >( limit & 0xffff ) },
            base_low { static_cast< std::uint16_t >( base & 0xffffff ) },
            access_right { access },
            limit_high { static_cast< std::uint8_t >( ( limit >> 16 ) & 0xf ) },
            flags { static_cast< std::uint8_t >( flag & 0xf ) },
            base_high { static_cast< std::uint8_t >( ( base >> 24 ) ) } {
        }
    };
    struct [[gnu::packed]] SystemSegmentDescriptor {
        std::uint64_t limit_low : 16;
        std::uint64_t base_low : 24;
        std::uint64_t access_right : 8;
        // struct [[gnu::packed]] Access {
        //     enum class Type : std::uint8_t {
        //         LDT = 0x2,
        //         TSS64_AVAILABLE = 0x9,
        //         TSS64_BUSY = 0xB
        //     } type : 4;
        //     std::uint8_t S : 1;
        //     std::uint8_t DPL : 2;
        //     std::uint8_t P : 1;
        // } access_right;
        std::uint64_t limit_high : 4;
        std::uint64_t flags : 4;
        std::uint64_t base_high : 40;
        std::uint64_t : 32;
        SystemSegmentDescriptor( std::uint64_t base, std::uint32_t limit, std::uint8_t access, std::uint8_t flag ) :
            limit_low { static_cast< std::uint16_t >( limit ) },
            base_low { static_cast< std::uint32_t >( base & 0xffffff ) },
            access_right { access },
            limit_high { static_cast< std::uint8_t >( ( limit >> 16 ) & 0xf ) },
            flags { static_cast< std::uint8_t >( flag & 0xf ) },
            base_high { static_cast< std::uint64_t >( ( base >> 24 ) & 0xffffffffff ) } {
        }
    };
    struct [[gnu::packed]] TaskStateSegment {
        // In Long Mode, the TSS does not store information on a task's execution state, instead it is used to store the Interrupt Stack Table.

        uint32_t reserved0;               // 4
        mutable uint64_t rsp[ 3 ];        // 24
        uint64_t reserved1;               // 4
        mutable uint64_t ist[ 7 ];        // 56
        uint64_t reserved2;               // 4
        uint16_t reserved3;               // 2
        uint16_t io_map_base_address;     // 2

        auto load_tr( ) const noexcept {
            __asm__ __volatile__( "ltr %%ax" ::"a"( SELECTOR_TSS ) : "memory" );
        }
        auto set_kstack( std::uint64_t kstack_top ) const {
            this->rsp[ 0 ] = kstack_top;
        }
        auto set_ist( std::uint64_t ist_index, std::uint64_t stack_top ) const {
            this->ist[ ist_index ] = stack_top;
        }
    };

private:
    GDT( void ) {};

public:
    static auto initialize( std::uint64_t core ) -> GDT *;

private:
    class [[gnu::packed]] DescriptorRegister final : public Utils::DescriptorRegister< SegmentDescriptor, 1024 > {
    public:
        DescriptorRegister( void ) :
            Utils::DescriptorRegister< SegmentDescriptor, 1024 > { nullptr } {
        }
        DescriptorRegister( SegmentDescriptor *_offset ) :
            Utils::DescriptorRegister< SegmentDescriptor, 1024 > { _offset } {
        }

    public:
        auto read( void ) -> SegmentDescriptor * {
            __asm__ __volatile__( "sgdt %0" : "=m"( *this ):: );
            return this->offset;
        }
        auto write( void ) -> void {
            __asm__ __volatile__( "lgdt %0" ::"m"( *this ) : );
            __asm__ __volatile__(
                "movq %%rax, %%ds \n\t"
                "movq %%rax, %%es \n\t"
                "movq %%rax, %%fs \n\t"
                "movq %%rax, %%gs \n\t"
                "movq %%rax, %%ss \n\t"
                "pushq %[SELECTOR_CODE64] \n\t"
                "leaq .next(%%rip),%%rax \n\t"
                "pushq %%rax \n\t"
                "lretq \n\r"
                ".next: \n\t" : : [SELECTOR_CODE64] "i"( SELECTOR_CODE64_KERNEL ),
                                  [SELECTOR_DATA64] "a"( SELECTOR_DATA64_KERNEL ) : );
        }
    };

    SegmentDescriptor segment_descriptors[ GDT_COUNT ][ 1024 ] { };     // 最高256个gdt每个cpu一个核心
    DescriptorRegister gdtrs[ GDT_COUNT ];
    TaskStateSegment tss[ GDT_COUNT ];

public:
    const auto &get_tss( std::uint64_t core ) const {
        return tss[ core ];
    }
} inline *gdt;
}     // namespace Memory