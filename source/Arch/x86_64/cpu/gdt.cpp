#include <Arch/x86_64/platform/platform.hpp>
#include <Kernel/print.hpp>
#include <Libcxx/cstring.hpp>
#include <Lib/spin_lock.hpp>
using namespace QuantumNEC;
using namespace Architecture;
using namespace Lib;
GlobalSegmentDescriptorTable::GlobalSegmentDescriptorTable( VOID ) noexcept :
    Descriptor< GlobalSegmentDescriptor, GLOBAL_SEGMENT_DESCRIPTOR_TABLE_COUNT, SEGMENT_DESCRIPTOR_COUNT > { } {
    for ( size_t i { }, tss_base_low { }, tss_base_high { }; i < __config.smp.cpu_count; ++i ) {
        tss_base_low = ( reinterpret_cast< uint64_t >( &this->tss[ i ] ) ) & 0xffffffff;
        tss_base_high = ( reinterpret_cast< uint64_t >( &this->tss[ i ] ) >> 32 ) & 0xffffffff;
        this->tss[ i ].set_io_map_base_address( static_cast< uint16_t >( sizeof( TaskStateSegmentDescriptor ) << 16 ) );
        // 设置GDT里的LDT
        this->descriptor_table[ i ][ 0 ].make( 0, 0, 0 );                                                                        /*0	        NULL descriptor                                                             0x00*/
        this->descriptor_table[ i ][ 1 ].make( 0, 0, AR_CODE64 );                                                                /*1	        KERNEL	Code	                                            64-bit	Segment 0x08*/
        this->descriptor_table[ i ][ 2 ].make( 0, 0, AR_DATA64 );                                                                /*2	        KERNEL	Data	                                            64-bit	Segment 0x10*/
        this->descriptor_table[ i ][ 3 ].make( 0, 0xfffff, AR_CODE32_DPL3 );                                                     /*3	        USER	Code	                                            32-bit	Segment 0x18*/
        this->descriptor_table[ i ][ 4 ].make( 0, 0xfffff, AR_DATA32_DPL3 );                                                     /*4	        USER	Data	                                            32-bit	Segment 0x20*/
        this->descriptor_table[ i ][ 5 ].make( 0, 0, AR_CODE64_DPL3 );                                                           /*5	        USER	Code	                                            64-bit	Segment 0x28*/
        this->descriptor_table[ i ][ 6 ].make( 0, 0, AR_DATA64_DPL3 );                                                           /*6	        USER	Data	                                            64-bit	Segment 0x30*/
        this->descriptor_table[ i ][ 7 ].make( 0, 0xfffff, AR_CODE32 );                                                          /*7	        KERNEL	Code	                                            32-bit	Segment	0x38*/
        this->descriptor_table[ i ][ 8 ].make( 0, 0xfffff, AR_DATA32 );                                                          /*8	        KERNEL	Data	                                            32-bit	Segment	0x40*/
        this->descriptor_table[ i ][ 9 ].make( 0, 0, 0 );                                                                        /*9	        NULL descriptor                                                             0x48*/
        this->descriptor_table[ i ][ 10 ].make( tss_base_low & 0xffffffff, sizeof( TaskStateSegmentDescriptor ) - 1, AR_TSS64 ); /*10 ~ 11   TSS (10->low 11->high, jmp one segment <9>)                 64-bit          0x50*/
        memcpy( &this->descriptor_table[ i ][ 11 ], &tss_base_high, 8 );
        // 还剩下13 ~ 8192个描述符保留
    }
    println< ostream::HeadLevel::SYSTEM >( "Loading the global segment descriptor table." );
    // 加载GDT
    this->load( );
    // 加载全局段中的TSS
    println< ostream::HeadLevel::SYSTEM >( "Loading the task state segment in global segment." );
    this->tss[ Apic::apic_id( ) ].load_tr( SELECTOR_TSS );
    println< ostream::HeadLevel::OK >( "Initialize the global segment descriptor table management." );
}
auto GlobalSegmentDescriptorTable::load( VOID ) CONST->VOID {
    ASM( "lgdt %[GDTR]" ::[ GDTR ] "m"( this->xdtr[ Apic::apic_id( ) ] ) );
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
auto GlobalSegmentDescriptorTable::read( VOID ) CONST->GlobalSegmentDescriptor * {
    ASM( "sgdt %0" ::"m"( this->xdtr[ Apic::apic_id( ) ] ) );
    return this->xdtr[ Apic::apic_id( ) ].descriptor;
}

auto GlobalSegmentDescriptor::make( IN uint64_t base,
                                    IN uint64_t limit,
                                    IN uint64_t access )
    -> GlobalSegmentDescriptor & {
    this->limit_low = limit & 0xffff;
    this->base_low = base & 0xffff;
    this->base_middle = ( base & 0xff0000 ) >> 16;
    this->access_right = access & 0xff;
    this->limit_high = ( ( limit >> 16 ) & 0xf ) | ( ( access >> 8 ) & 0xf0 );
    this->base_high = ( base >> 24 ) & 0xff;
    return *this;
}