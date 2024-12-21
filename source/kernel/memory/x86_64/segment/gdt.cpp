#include <kernel/interrupt/interrupt.hpp>
#include <kernel/memory/page/page_allocater.hpp>
#include <kernel/memory/x86_64/segment/gdt.hpp>
#include <kernel/print.hpp>
#include <lib/spin_lock.hpp>
#include <libcxx/cstring.hpp>
using namespace std;
namespace QuantumNEC::Kernel::x86_64 {
inline static GlobalSegmentDescriptorTable::_GDT _gdt { };
GlobalSegmentDescriptorTable::GlobalSegmentDescriptorTable( void ) noexcept {
    this->gdt = new ( &_gdt ) GlobalSegmentDescriptorTable::_GDT { };
    for ( auto &task_state_segment : this->gdt->tss ) {
        task_state_segment.set_io_map_base_address( static_cast< uint16_t >( sizeof( TaskStateSegmentDescriptor ) << 16 ) );
    }
    for ( auto i = 0ul; auto &descriptor_groub : this->gdt->descriptor_table ) {
        std::memset( descriptor_groub, 0, sizeof *descriptor_groub );
        /*0 NULL descriptor             0x00*/
        descriptor_groub[ 0 ].make( 0, 0, 0 );
        /*1 KERNEL	Code 64-bit	Segment 0x08*/
        descriptor_groub[ 1 ].make( 0, 0, AR_CODE64 );
        /*2 KERNEL	Data 64-bit	Segment 0x10*/
        descriptor_groub[ 2 ].make( 0, 0, AR_DATA64 );
        /*3 USER	Code 32-bit	Segment 0x18*/
        descriptor_groub[ 3 ].make( 0, 0xfffff, AR_CODE32_DPL3 );
        /*4 USER	Data 32-bit	Segment 0x20*/
        descriptor_groub[ 4 ].make( 0, 0xfffff, AR_DATA32_DPL3 );
        /*5 USER	Code 64-bit	Segment 0x28*/
        descriptor_groub[ 5 ].make( 0, 0, AR_CODE64_DPL3 );
        /*6 USER	Data 64-bit	Segment 0x30*/
        descriptor_groub[ 6 ].make( 0, 0, AR_DATA64_DPL3 );
        /*7 KERNEL	Code 32-bit	Segment	0x38*/
        descriptor_groub[ 7 ].make( 0, 0xfffff, AR_CODE32 );
        /*8 KERNEL	Data 32-bit	Segment	0x40*/
        descriptor_groub[ 8 ].make( 0, 0xfffff, AR_DATA32 );
        /*9 NULL descriptor 0x48*/
        descriptor_groub[ 9 ].make( 0, 0, 0 );

        auto tss_base_low  = ( reinterpret_cast< uint64_t >( &this->gdt->tss[ i ] ) ) & 0xffffffff;
        auto tss_base_high = ( reinterpret_cast< uint64_t >( &this->gdt->tss[ i++ ] ) >> 32 ) & 0xffffffff;
        /*10 ~ 11   TSS (10->low 11->high, jmp one segment <9>)                 64-bit          0x50*/
        descriptor_groub[ 10 ].make( tss_base_low & 0xffffffff, sizeof( TaskStateSegmentDescriptor ) - 1, AR_TSS64 );
        std::memcpy( &descriptor_groub[ 11 ], &tss_base_high, 8 );
    }

    // 加载GDT
    this->gdt->load( 0 );
    // 加载全局段中的TSS
    this->gdt->tss[ 0 ].load_tr( SELECTOR_TSS );
    println< print_level::OK >( "Initialize segment." );
}
auto GlobalSegmentDescriptorTable::_GDT::load( IN uint64_t processor_id ) const -> void {
    ASM( "LGDT %[GDTR]" ::[ GDTR ] "m"( this->xdtr[ processor_id ] ) );
    ASM(
        "MOVQ %%RAX, %%DS \n\t"
        "MOVQ %%RAX, %%ES \n\t"
        "MOVQ %%RAX, %%FS \n\t"
        "MOVQ %%RAX, %%GS \n\t"
        "MOVQ %%RAX, %%SS \n\t"
        "PUSHQ %[SELECTOR_CODE64] \n\t"
        "LEAQ .next(%%RIP),%%RAX \n\t"
        "PUSHQ %%RAX \n\t"
        "LRETQ \n\r"
        ".next: \n\t"
        :
        : [SELECTOR_CODE64] "i"( SELECTOR_CODE64_KERNEL ),
          [SELECTOR_DATA64] "a"( SELECTOR_DATA64_KERNEL )
        : );
    return;
}
auto GlobalSegmentDescriptorTable::_GDT::read( IN uint64_t processor_id ) const -> GlobalSegmentDescriptor * {
    ASM( "sgdt %0" ::"m"( this->xdtr[ processor_id ] ) );
    return this->xdtr[ processor_id ].descriptor;
}

auto GlobalSegmentDescriptor::make( IN uint64_t base,
                                    IN uint64_t limit,
                                    IN uint64_t access )
    -> GlobalSegmentDescriptor & {
    this->limit_low    = limit & 0xffff;
    this->base_low     = base & 0xffff;
    this->base_middle  = ( base & 0xff0000 ) >> 16;
    this->access_right = access & 0xff;
    this->limit_high   = ( ( limit >> 16 ) & 0xf ) | ( ( access >> 8 ) & 0xf0 );
    this->base_high    = ( base >> 24 ) & 0xff;
    return *this;
}
}     // namespace QuantumNEC::Kernel::x86_64