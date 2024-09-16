#include <Arch/x86_64/platform/platform.hpp>
#include <Lib/IO/Stream/iostream>
#include <Lib/STL/cstring>

using namespace QuantumNEC;
using namespace QuantumNEC::Architecture;
using namespace QuantumNEC::Lib::IO;
/**
 * @brief 中断入口函数表
 */
PUBLIC Architecture::InterruptDescriptorTable::InterruptEntry interrupt_entry_table[ INTERRUPT_DESCRIPTOR_COUNT ];

PUBLIC Lib::Types::Ptr< CONST Lib::Types::char_t > interrupt_name[ INTERRUPT_DESCRIPTOR_COUNT ];     // 各个异常的名字

_C_LINK ASMCALL auto general_interrupt_handler( IN Lib::Types::Ptr< CONST InterruptDescriptorTable::InterruptFrame > frame ) -> Lib::Types::Ptr< CONST InterruptDescriptorTable::InterruptFrame > {
#ifndef APIC
    if ( frame->vector == 0x27 || frame->vector == 0x2f )     // 0x2f是从片8259A上的最后一个irq引脚，保留
        return frame;                                         // IRQ7和IRQ15会产生伪中断(spurious interrupt),无须处理。
#endif
    if ( interrupt_entry_table[ frame->vector ] == general_interrupt_handler ) {
        // 显示中断名
        sout[ ostream::HeadLevel::ERROR ] << "IRQ_" << frame->vector << ":" << ::interrupt_name[ frame->vector ] << endl;
        // 显示各个寄存器值
        sout[ ostream::HeadLevel::INFO ].printk( DisplayColor::WHITE, DisplayColor::BLACK, "Rflags -> %#018lx\n\r", frame->rflags );
        sout[ ostream::HeadLevel::INFO ].printk( DisplayColor::WHITE, DisplayColor::BLACK, "RIP -> %#018lx\tRSP -> %#018lx\tSS  -> %#018lx\tCS  -> %#018lx\n\r", frame->rip, frame->rsp, frame->ss, frame->cs );
        InterruptDescriptorTable::InterruptDescriptorTable::display_registers( &frame->regs );
        if ( frame->error_code ) {
            sout[ ostream::HeadLevel::INFO ] << "Error Code -> " << frame->error_code << endl;
        }
        /* #TS #NP #SS #GP特殊处理*/
        if ( frame->vector == 0x0a || frame->vector == 0x0b || frame->vector == 0x0c || frame->vector == 0x0d ) {
            if ( frame->error_code & 0x01 ) {
                sout[ ostream::HeadLevel::INFO ] << "The exception occurred during delivery of an event external to the program,such as an interrupt or an earlier exception." << endl;
            }
            if ( frame->error_code & 0x02 ) {
                sout[ ostream::HeadLevel::INFO ] << "Refers to a gate descriptor in the IDT." << endl;
            }
            else {
                sout[ ostream::HeadLevel::INFO ] << "Refers to a descriptor in the GDT or the current LDT." << endl;
            }
            if ( !( frame->error_code & 0x02 ) ) {
                if ( frame->error_code & 0x04 ) {
                    sout[ ostream::HeadLevel::INFO ] << "Refers to a segment or gate descriptor in the LDT." << endl;
                }
                else {
                    sout[ ostream::HeadLevel::INFO ] << "Refers to a descriptor in the current GDT." << endl;
                }
            }
            sout[ ostream::HeadLevel::INFO ].printk( DisplayColor::WHITE, DisplayColor::BLACK, "Segment Selector Index : %#010x\n", frame->error_code & 0xfff8 );
        }

        if ( frame->vector == 0x0e ) /* #PF的特殊处理 */
        {
            sout[ ostream::HeadLevel::INFO ];
            if ( !( frame->error_code & 0x01 ) ) {
                sout << "Page Not-Present, ";
            }
            if ( frame->error_code & 0x02 ) {
                sout << "Write Cause Fault, ";
            }
            else {
                sout << "Read Cause Fault, ";
            }
            if ( frame->error_code & 0x04 ) {
                sout << "Fault in user(3), ";
            }
            else {
                sout << "Fault in supervisor(0,1,2), ";
            }
            if ( frame->error_code & 0x08 ) {
                sout << "Reserved Bit Cause Fault, ";
            }
            if ( frame->error_code & 0x10 ) {
                sout << "Instruction fetch Cause Fault";
            }
            endl( sout );
        }
        while ( TRUE )     // 因为进入异常处理时已经关闭了中断所以直接阻塞即可
            Architecture::CPUs::hlt( );
    }
    else {
        return interrupt_entry_table[ frame->vector ]( frame );
    }
}
/**
 * @brief 定义异常接收函数
 */

/**
 * @brief 设置中断入口（Trap）
 */

#define SET_TRAP_HANDLER( index, ist )                                      \
    ASM( "LEAQ InterruptHandler" #index "(%%RIP), %0" : "=r"( function ) ); \
    this->descriptor_table[ 0 ][ index ].make(                              \
        function,                                                           \
        SELECTOR_CODE64_KERNEL,                                             \
        ist,                                                                \
        static_cast< Lib::Types::uint8_t >( InterruptDescriptorAttribute::TRAP ) )
/**
 * @brief 设置中断入口（System）
 */
#define SET_SYSTEM_HANDLER( index, ist )                                    \
    ASM( "LEAQ InterruptHandler" #index "(%%RIP), %0" : "=r"( function ) ); \
    this->descriptor_table[ 0 ][ index ].make(                              \
        function,                                                           \
        SELECTOR_CODE64_KERNEL,                                             \
        ist,                                                                \
        static_cast< Lib::Types::uint8_t >( InterruptDescriptorAttribute::SYSTEM ) )
/**
 * @brief 设置中断入口（System——Interrupt）
 */
#define SET_SYSTEM_INTERRUPT_HANDLER( index, ist )                          \
    ASM( "LEAQ InterruptHandler" #index "(%%RIP), %0" : "=r"( function ) ); \
    this->descriptor_table[ 0 ][ index ].make(                              \
        function,                                                           \
        SELECTOR_CODE64_KERNEL,                                             \
        ist,                                                                \
        static_cast< Lib::Types::uint8_t >( InterruptDescriptorAttribute::SYSTEM_INTERRUPT ) )
/**
 * @brief 设置中断入口（Interrupt）
 */
#define SET_INTERRUPT_HANDLER( index, ist )                                 \
    ASM( "LEAQ InterruptHandler" #index "(%%RIP), %0" : "=r"( function ) ); \
    this->descriptor_table[ 0 ][ index ].make(                              \
        function,                                                           \
        SELECTOR_CODE64_KERNEL,                                             \
        ist,                                                                \
        static_cast< Lib::Types::uint8_t >( InterruptDescriptorAttribute::INTERRUPT ) )
PUBLIC namespace QuantumNEC::Architecture {
    InterruptDescriptorTable::InterruptDescriptorTable( VOID ) noexcept :
        Descriptor< InterruptDescriptor, INTERRUPT_DESCRIPTOR_TABLE_COUNT, INTERRUPT_DESCRIPTOR_COUNT > { } {
        Lib::IO::sout[ Lib::IO::ostream::HeadLevel::START ] << "Initialize the interrupt descriptor table management." << Lib::IO::endl;
        Architecture::CPUs::cli( );     // 关中断
        Lib::STL::memset( this->xdtr.descriptor, 0, INTERRUPT_DESCRIPTOR_COUNT * sizeof( InterruptDescriptor ) );

        for ( Lib::Types::uint16_t i { }; i < INTERRUPT_DESCRIPTOR_COUNT; ++i ) {
            interrupt_name[ i ] = "unknown";                            // 先统一赋值为unknown
            interrupt_entry_table[ i ] = general_interrupt_handler;     // 默认为general_interrupt_handler
        }
        interrupt_name[ 0 ] = "#DE Divide-by-zero Error";
        interrupt_name[ 1 ] = "#DB Debug";
        interrupt_name[ 2 ] = "--- Non Maskable Interrupt(NMI)";
        interrupt_name[ 3 ] = "#BP Breakpoint";
        interrupt_name[ 4 ] = "#OF Overflow";
        interrupt_name[ 5 ] = "#BR Bound Range Exceeded Exception";
        interrupt_name[ 6 ] = "#UD Invalid Opcode Exception";
        interrupt_name[ 7 ] = "#NM Device Not Available Exception";
        interrupt_name[ 8 ] = "#DF Double Fault Exception";
        interrupt_name[ 9 ] = "--- Coprocessor Segment Overrun";
        interrupt_name[ 10 ] = "#TS Invalid TSS Exception";
        interrupt_name[ 11 ] = "#NP Segment Not Present";
        interrupt_name[ 12 ] = "#SS Stack-Segment Fault Exception";
        interrupt_name[ 13 ] = "#GP General Protection Exception";
        interrupt_name[ 14 ] = "#PF Page Fault Exception";
        interrupt_name[ 15 ] = "--- Intel Reserved";     // 第15项是intel保留项，未使用
        interrupt_name[ 16 ] = "#MF x87 Floating-Point Exception";
        interrupt_name[ 17 ] = "#AC Alignment Check";
        interrupt_name[ 18 ] = "#MC Machine Check Exception";
        interrupt_name[ 19 ] = "#XF SIMD Floating-Point Exception";
        interrupt_name[ 20 ] = "#CP Control Protection Exception";
        for ( Lib::Types::uint16_t i { 21 }; i < 27; ++i ) {
            interrupt_name[ i ] = "--- System Reserved";
        }
        interrupt_name[ 27 ] = "#HV Exception Injected By The Virtual Machine";
        interrupt_name[ 28 ] = "#VC VMM Communication Failed";
        interrupt_name[ 29 ] = "#SX Security Exception";

        /* 注册所有入口函数到中断描述符表 */
        sout[ ostream::HeadLevel::INFO ] << "Setting the interrupt handler entry for the interrupt descriptor table" << endl;
        Lib::Types::uint64_t function { };
        SET_TRAP_HANDLER( 0x00, 0 );
        SET_TRAP_HANDLER( 0x01, 0 );
        SET_INTERRUPT_HANDLER( 0x02, 0 );
        SET_SYSTEM_HANDLER( 0x03, 0 );
        SET_SYSTEM_HANDLER( 0x04, 0 );
        SET_SYSTEM_HANDLER( 0x05, 0 );
        SET_TRAP_HANDLER( 0x06, 0 );
        SET_TRAP_HANDLER( 0x07, 0 );
        SET_TRAP_HANDLER( 0x08, 0 );
        SET_TRAP_HANDLER( 0x09, 0 );
        SET_TRAP_HANDLER( 0x0a, 0 );
        SET_TRAP_HANDLER( 0x0b, 0 );
        SET_TRAP_HANDLER( 0x0c, 0 );
        SET_TRAP_HANDLER( 0x0d, 0 );
        SET_TRAP_HANDLER( 0x0e, 0 );
        SET_TRAP_HANDLER( 0x0f, 0 );
        SET_TRAP_HANDLER( 0x10, 0 );
        SET_TRAP_HANDLER( 0x11, 0 );
        SET_TRAP_HANDLER( 0x12, 0 );
        SET_TRAP_HANDLER( 0x13, 0 );
        SET_TRAP_HANDLER( 0x14, 0 );
        SET_TRAP_HANDLER( 0x15, 0 );
        SET_TRAP_HANDLER( 0x16, 0 );
        SET_TRAP_HANDLER( 0x17, 0 );
        SET_TRAP_HANDLER( 0x18, 0 );
        SET_TRAP_HANDLER( 0x19, 0 );
        SET_TRAP_HANDLER( 0x1a, 0 );
        SET_TRAP_HANDLER( 0x1b, 0 );
        SET_TRAP_HANDLER( 0x1c, 0 );
        SET_TRAP_HANDLER( 0x1d, 0 );
        SET_TRAP_HANDLER( 0x1e, 0 );
        SET_TRAP_HANDLER( 0x1f, 0 );
        SET_INTERRUPT_HANDLER( 0x20, 0 );
        SET_INTERRUPT_HANDLER( 0x21, 0 );
        SET_INTERRUPT_HANDLER( 0x22, 0 );
        SET_INTERRUPT_HANDLER( 0x23, 0 );
        SET_INTERRUPT_HANDLER( 0x24, 0 );
        SET_INTERRUPT_HANDLER( 0x25, 0 );
        SET_INTERRUPT_HANDLER( 0x26, 0 );
        SET_INTERRUPT_HANDLER( 0x27, 0 );
        SET_INTERRUPT_HANDLER( 0x28, 0 );
        SET_INTERRUPT_HANDLER( 0x29, 0 );
        SET_INTERRUPT_HANDLER( 0x2a, 0 );
        SET_INTERRUPT_HANDLER( 0x2b, 0 );
        SET_INTERRUPT_HANDLER( 0x2c, 0 );
        SET_INTERRUPT_HANDLER( 0x2d, 0 );
        SET_INTERRUPT_HANDLER( 0x2e, 0 );
        SET_INTERRUPT_HANDLER( 0x2f, 0 );
        SET_INTERRUPT_HANDLER( 0x30, 0 );
        SET_INTERRUPT_HANDLER( 0x31, 0 );
        SET_INTERRUPT_HANDLER( 0x32, 0 );
        SET_INTERRUPT_HANDLER( 0x33, 0 );
        SET_INTERRUPT_HANDLER( 0x34, 0 );
        SET_INTERRUPT_HANDLER( 0x35, 0 );
        SET_INTERRUPT_HANDLER( 0x36, 0 );
        SET_INTERRUPT_HANDLER( 0x37, 0 );
        SET_INTERRUPT_HANDLER( 0x38, 0 );
        SET_INTERRUPT_HANDLER( 0x39, 0 );
        SET_INTERRUPT_HANDLER( 0x3a, 0 );
        SET_INTERRUPT_HANDLER( 0x3b, 0 );
        SET_INTERRUPT_HANDLER( 0x3c, 0 );
        SET_INTERRUPT_HANDLER( 0x3d, 0 );
        SET_INTERRUPT_HANDLER( 0x3e, 0 );
        SET_INTERRUPT_HANDLER( 0x3f, 0 );
        SET_INTERRUPT_HANDLER( 0x40, 0 );
        SET_INTERRUPT_HANDLER( 0x41, 0 );
        SET_INTERRUPT_HANDLER( 0x42, 0 );
        SET_INTERRUPT_HANDLER( 0x43, 0 );
        SET_INTERRUPT_HANDLER( 0x44, 0 );
        SET_INTERRUPT_HANDLER( 0x45, 0 );
        SET_INTERRUPT_HANDLER( 0x46, 0 );
        SET_INTERRUPT_HANDLER( 0x47, 0 );
        SET_INTERRUPT_HANDLER( 0x48, 0 );
        SET_INTERRUPT_HANDLER( 0x49, 0 );
        SET_INTERRUPT_HANDLER( 0x4a, 0 );
        SET_INTERRUPT_HANDLER( 0x4b, 0 );
        SET_INTERRUPT_HANDLER( 0x4c, 0 );
        SET_INTERRUPT_HANDLER( 0x4d, 0 );
        SET_INTERRUPT_HANDLER( 0x4e, 0 );
        SET_INTERRUPT_HANDLER( 0x4f, 0 );
        SET_INTERRUPT_HANDLER( 0x50, 0 );
        SET_INTERRUPT_HANDLER( 0x51, 0 );
        SET_INTERRUPT_HANDLER( 0x52, 0 );
        SET_INTERRUPT_HANDLER( 0x53, 0 );
        SET_INTERRUPT_HANDLER( 0x54, 0 );
        SET_INTERRUPT_HANDLER( 0x55, 0 );
        SET_INTERRUPT_HANDLER( 0x56, 0 );
        SET_INTERRUPT_HANDLER( 0x57, 0 );
        SET_INTERRUPT_HANDLER( 0x58, 0 );
        SET_INTERRUPT_HANDLER( 0x59, 0 );
        SET_INTERRUPT_HANDLER( 0x5a, 0 );
        SET_INTERRUPT_HANDLER( 0x5b, 0 );
        SET_INTERRUPT_HANDLER( 0x5c, 0 );
        SET_INTERRUPT_HANDLER( 0x5d, 0 );
        SET_INTERRUPT_HANDLER( 0x5e, 0 );
        SET_INTERRUPT_HANDLER( 0x5f, 0 );
        SET_INTERRUPT_HANDLER( 0x60, 0 );
        SET_INTERRUPT_HANDLER( 0x61, 0 );
        SET_INTERRUPT_HANDLER( 0x62, 0 );
        SET_INTERRUPT_HANDLER( 0x63, 0 );
        SET_INTERRUPT_HANDLER( 0x64, 0 );
        SET_INTERRUPT_HANDLER( 0x65, 0 );
        SET_INTERRUPT_HANDLER( 0x66, 0 );
        SET_INTERRUPT_HANDLER( 0x67, 0 );
        SET_INTERRUPT_HANDLER( 0x68, 0 );
        SET_INTERRUPT_HANDLER( 0x69, 0 );
        SET_INTERRUPT_HANDLER( 0x6a, 0 );
        SET_INTERRUPT_HANDLER( 0x6b, 0 );
        SET_INTERRUPT_HANDLER( 0x6c, 0 );
        SET_INTERRUPT_HANDLER( 0x6d, 0 );
        SET_INTERRUPT_HANDLER( 0x6e, 0 );
        SET_INTERRUPT_HANDLER( 0x6f, 0 );
        SET_INTERRUPT_HANDLER( 0x70, 0 );
        SET_INTERRUPT_HANDLER( 0x71, 0 );
        SET_INTERRUPT_HANDLER( 0x72, 0 );
        SET_INTERRUPT_HANDLER( 0x73, 0 );
        SET_INTERRUPT_HANDLER( 0x74, 0 );
        SET_INTERRUPT_HANDLER( 0x75, 0 );
        SET_INTERRUPT_HANDLER( 0x76, 0 );
        SET_INTERRUPT_HANDLER( 0x77, 0 );
        SET_INTERRUPT_HANDLER( 0x78, 0 );
        SET_INTERRUPT_HANDLER( 0x79, 0 );
        SET_INTERRUPT_HANDLER( 0x7a, 0 );
        SET_INTERRUPT_HANDLER( 0x7b, 0 );
        SET_INTERRUPT_HANDLER( 0x7c, 0 );
        SET_INTERRUPT_HANDLER( 0x7d, 0 );
        SET_INTERRUPT_HANDLER( 0x7e, 0 );
        SET_INTERRUPT_HANDLER( 0x7f, 0 );
        SET_SYSTEM_INTERRUPT_HANDLER( 0x80, 0 );     // 系统调用
        SET_INTERRUPT_HANDLER( 0x81, 0 );
        SET_INTERRUPT_HANDLER( 0x82, 0 );
        SET_INTERRUPT_HANDLER( 0x83, 0 );
        SET_INTERRUPT_HANDLER( 0x84, 0 );
        SET_INTERRUPT_HANDLER( 0x85, 0 );
        SET_INTERRUPT_HANDLER( 0x86, 0 );
        SET_INTERRUPT_HANDLER( 0x87, 0 );
        SET_INTERRUPT_HANDLER( 0x88, 0 );
        SET_INTERRUPT_HANDLER( 0x89, 0 );
        SET_INTERRUPT_HANDLER( 0x8a, 0 );
        SET_INTERRUPT_HANDLER( 0x8b, 0 );
        SET_INTERRUPT_HANDLER( 0x8c, 0 );
        SET_INTERRUPT_HANDLER( 0x8d, 0 );
        SET_INTERRUPT_HANDLER( 0x8e, 0 );
        SET_INTERRUPT_HANDLER( 0x8f, 0 );
        SET_INTERRUPT_HANDLER( 0x90, 0 );
        SET_INTERRUPT_HANDLER( 0x91, 0 );
        SET_INTERRUPT_HANDLER( 0x92, 0 );
        SET_INTERRUPT_HANDLER( 0x93, 0 );
        SET_INTERRUPT_HANDLER( 0x94, 0 );
        SET_INTERRUPT_HANDLER( 0x95, 0 );
        SET_INTERRUPT_HANDLER( 0x96, 0 );
        SET_INTERRUPT_HANDLER( 0x97, 0 );
        SET_INTERRUPT_HANDLER( 0x98, 0 );
        SET_INTERRUPT_HANDLER( 0x99, 0 );
        SET_INTERRUPT_HANDLER( 0x9a, 0 );
        SET_INTERRUPT_HANDLER( 0x9b, 0 );
        SET_INTERRUPT_HANDLER( 0x9c, 0 );
        SET_INTERRUPT_HANDLER( 0x9d, 0 );
        SET_INTERRUPT_HANDLER( 0x9e, 0 );
        SET_INTERRUPT_HANDLER( 0x9f, 0 );
        SET_INTERRUPT_HANDLER( 0xa0, 0 );
        SET_INTERRUPT_HANDLER( 0xa1, 0 );
        SET_INTERRUPT_HANDLER( 0xa2, 0 );
        SET_INTERRUPT_HANDLER( 0xa3, 0 );
        SET_INTERRUPT_HANDLER( 0xa4, 0 );
        SET_INTERRUPT_HANDLER( 0xa5, 0 );
        SET_INTERRUPT_HANDLER( 0xa6, 0 );
        SET_INTERRUPT_HANDLER( 0xa7, 0 );
        SET_INTERRUPT_HANDLER( 0xa8, 0 );
        SET_INTERRUPT_HANDLER( 0xa9, 0 );
        SET_INTERRUPT_HANDLER( 0xaa, 0 );
        SET_INTERRUPT_HANDLER( 0xab, 0 );
        SET_INTERRUPT_HANDLER( 0xac, 0 );
        SET_INTERRUPT_HANDLER( 0xad, 0 );
        SET_INTERRUPT_HANDLER( 0xae, 0 );
        SET_INTERRUPT_HANDLER( 0xaf, 0 );
        SET_INTERRUPT_HANDLER( 0xb0, 0 );
        SET_INTERRUPT_HANDLER( 0xb1, 0 );
        SET_INTERRUPT_HANDLER( 0xb2, 0 );
        SET_INTERRUPT_HANDLER( 0xb3, 0 );
        SET_INTERRUPT_HANDLER( 0xb4, 0 );
        SET_INTERRUPT_HANDLER( 0xb5, 0 );
        SET_INTERRUPT_HANDLER( 0xb6, 0 );
        SET_INTERRUPT_HANDLER( 0xb7, 0 );
        SET_INTERRUPT_HANDLER( 0xb8, 0 );
        SET_INTERRUPT_HANDLER( 0xb9, 0 );
        SET_INTERRUPT_HANDLER( 0xba, 0 );
        SET_INTERRUPT_HANDLER( 0xbb, 0 );
        SET_INTERRUPT_HANDLER( 0xbc, 0 );
        SET_INTERRUPT_HANDLER( 0xbd, 0 );
        SET_INTERRUPT_HANDLER( 0xbe, 0 );
        SET_INTERRUPT_HANDLER( 0xbf, 0 );
        SET_INTERRUPT_HANDLER( 0xc0, 0 );
        SET_INTERRUPT_HANDLER( 0xc1, 0 );
        SET_INTERRUPT_HANDLER( 0xc2, 0 );
        SET_INTERRUPT_HANDLER( 0xc3, 0 );
        SET_INTERRUPT_HANDLER( 0xc4, 0 );
        SET_INTERRUPT_HANDLER( 0xc5, 0 );
        SET_INTERRUPT_HANDLER( 0xc6, 0 );
        SET_INTERRUPT_HANDLER( 0xc7, 0 );
        SET_INTERRUPT_HANDLER( 0xc8, 0 );
        SET_INTERRUPT_HANDLER( 0xc9, 0 );
        SET_INTERRUPT_HANDLER( 0xca, 0 );
        SET_INTERRUPT_HANDLER( 0xcb, 0 );
        SET_INTERRUPT_HANDLER( 0xcc, 0 );
        SET_INTERRUPT_HANDLER( 0xcd, 0 );
        SET_INTERRUPT_HANDLER( 0xce, 0 );
        SET_INTERRUPT_HANDLER( 0xcf, 0 );
        SET_INTERRUPT_HANDLER( 0xd0, 0 );
        SET_INTERRUPT_HANDLER( 0xd1, 0 );
        SET_INTERRUPT_HANDLER( 0xd2, 0 );
        SET_INTERRUPT_HANDLER( 0xd3, 0 );
        SET_INTERRUPT_HANDLER( 0xd4, 0 );
        SET_INTERRUPT_HANDLER( 0xd5, 0 );
        SET_INTERRUPT_HANDLER( 0xd6, 0 );
        SET_INTERRUPT_HANDLER( 0xd7, 0 );
        SET_INTERRUPT_HANDLER( 0xd8, 0 );
        SET_INTERRUPT_HANDLER( 0xd9, 0 );
        SET_INTERRUPT_HANDLER( 0xda, 0 );
        SET_INTERRUPT_HANDLER( 0xdb, 0 );
        SET_INTERRUPT_HANDLER( 0xdc, 0 );
        SET_INTERRUPT_HANDLER( 0xdd, 0 );
        SET_INTERRUPT_HANDLER( 0xde, 0 );
        SET_INTERRUPT_HANDLER( 0xdf, 0 );
        SET_INTERRUPT_HANDLER( 0xe0, 0 );
        SET_INTERRUPT_HANDLER( 0xe1, 0 );
        SET_INTERRUPT_HANDLER( 0xe2, 0 );
        SET_INTERRUPT_HANDLER( 0xe3, 0 );
        SET_INTERRUPT_HANDLER( 0xe4, 0 );
        SET_INTERRUPT_HANDLER( 0xe5, 0 );
        SET_INTERRUPT_HANDLER( 0xe6, 0 );
        SET_INTERRUPT_HANDLER( 0xe7, 0 );
        SET_INTERRUPT_HANDLER( 0xe8, 0 );
        SET_INTERRUPT_HANDLER( 0xe9, 0 );
        SET_INTERRUPT_HANDLER( 0xea, 0 );
        SET_INTERRUPT_HANDLER( 0xeb, 0 );
        SET_INTERRUPT_HANDLER( 0xec, 0 );
        SET_INTERRUPT_HANDLER( 0xed, 0 );
        SET_INTERRUPT_HANDLER( 0xee, 0 );
        SET_INTERRUPT_HANDLER( 0xef, 0 );
        SET_INTERRUPT_HANDLER( 0xf0, 0 );
        SET_INTERRUPT_HANDLER( 0xf1, 0 );
        SET_INTERRUPT_HANDLER( 0xf2, 0 );
        SET_INTERRUPT_HANDLER( 0xf3, 0 );
        SET_INTERRUPT_HANDLER( 0xf4, 0 );
        SET_INTERRUPT_HANDLER( 0xf5, 0 );
        SET_INTERRUPT_HANDLER( 0xf6, 0 );
        SET_INTERRUPT_HANDLER( 0xf7, 0 );
        SET_INTERRUPT_HANDLER( 0xf8, 0 );
        SET_INTERRUPT_HANDLER( 0xf9, 0 );
        SET_INTERRUPT_HANDLER( 0xfa, 0 );
        SET_INTERRUPT_HANDLER( 0xfb, 0 );
        SET_INTERRUPT_HANDLER( 0xfc, 0 );
        SET_INTERRUPT_HANDLER( 0xfd, 0 );
        SET_INTERRUPT_HANDLER( 0xfe, 0 );
        SET_INTERRUPT_HANDLER( 0xff, 0 );
        /* 挂载 idt*/
        sout[ ostream::HeadLevel::SYSTEM ] << "Loading the interrupt descriptor table." << endl;
        this->load( );

        Lib::IO::sout[ Lib::IO::ostream::HeadLevel::OK ] << "Initialize the interrupt descriptor table management." << Lib::IO::endl;
    }

    auto InterruptDescriptorTable::load( VOID ) CONST->VOID {
        ASM( "lidt %0" ::"m"( this->xdtr ) );
        return;
    }
    auto InterruptDescriptorTable::read( VOID ) CONST->Lib::Types::Ptr< InterruptDescriptor > {
        // 获取idt地址并返回
        ASM( "sidt %0" ::"m"( this->xdtr ) );
        return this->xdtr.descriptor;
    }

    auto InterruptDescriptorTable::display_registers( IN Lib::Types::Ptr< CONST RegisterFrame > registers )->VOID {
        using namespace ::QuantumNEC::Lib::IO;
        sout[ ostream::HeadLevel::INFO ].printk( DisplayColor::WHITE, DisplayColor::BLACK, "RAX -> %#018lx\tRBX -> %#018lx\tRCX -> %#018lx\tRDX -> %#018lx\n\r", registers->rax, registers->rbx, registers->rcx, registers->rdx );
        sout[ ostream::HeadLevel::INFO ].printk( DisplayColor::WHITE, DisplayColor::BLACK, "R8  -> %#018lx\tR9  -> %#018lx\tR10 -> %#018lx\tR11 -> %#018lx\tR12 -> %#018lx\tR13 -> %#018lx\tR14 -> %#018lx\tR15 -> %#018lx\n\r", registers->r8, registers->r9, registers->r10, registers->r11, registers->r12, registers->r13, registers->r14, registers->r15 );
        sout[ ostream::HeadLevel::INFO ].printk( DisplayColor::WHITE, DisplayColor::BLACK, "RDI -> %#018lx\tRSI -> %#018lx\tRBP -> %#018lx\n\r", registers->rdi, registers->rsi, registers->rbp );
        ControlRegisterFrame control_registers_frame { };
        sout[ ostream::HeadLevel::INFO ].printk( DisplayColor::WHITE, DisplayColor::BLACK, "CR0 -> %#018lx\tCR2 -> %#018lx\tCR3 -> %#018lx\tCR4 -> %#018lx\n\tCR8 -> %#018lx\r ", control_registers_frame.cr0, control_registers_frame.cr2, control_registers_frame.cr3, control_registers_frame.cr4, control_registers_frame.cr8 );
        sout[ ostream::HeadLevel::INFO ].printk( DisplayColor::WHITE, DisplayColor::BLACK, "DS -> %#018lx\tES -> %#018lx\tFS -> %#018lx\tGS -> %#018lx\n\r", registers->ds, registers->es, registers->fs, registers->gs );
        return;
    }
    auto InterruptDescriptorTable::set_interrupt_handler( IN CONST Lib::Types::uint8_t irq, IN InterruptEntry handle )->VOID {
        if ( irq > 0x1d ) {
            interrupt_entry_table[ irq ] = handle;
        }
        return;
    }
    auto InterruptDescriptorTable::set_exception_handler( IN CONST Lib::Types::uint8_t irq, IN InterruptEntry handle )->VOID {
        if ( irq < 0x1e ) {
            interrupt_entry_table[ irq ] = handle;
        }
        return;
    }
    GlobalSegmentDescriptorTable::GlobalSegmentDescriptorTable( VOID ) noexcept :
        Descriptor< GlobalSegmentDescriptor, GLOBAL_SEGMENT_DESCRIPTOR_TABLE_COUNT, SEGMENT_DESCRIPTOR_COUNT > { } {
        Lib::IO::sout[ Lib::IO::ostream::HeadLevel::START ] << "Initialize the global segment descriptor table management." << endl;
        for ( Lib::Types::size_t i { }, tss_base_low { }, tss_base_high { }; i < GLOBAL_SEGMENT_DESCRIPTOR_TABLE_COUNT; ++i ) {
            tss_base_low = ( reinterpret_cast< Lib::Types::uint64_t >( &this->tss[ i ] ) ) & 0xffffffff;
            tss_base_high = ( reinterpret_cast< Lib::Types::uint64_t >( &this->tss[ i ] ) >> 32 ) & 0xffffffff;
            this->tss[ i ].set_io_map_base_address( static_cast< Lib::Types::uint16_t >( sizeof( TaskStateSegmentDescriptor ) << 16 ) );
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
            Lib::STL::memcpy( &this->descriptor_table[ i ][ 11 ], &tss_base_high, 8 );
            // 还剩下13 ~ 8192个描述符保留
        }
        sout[ ostream::HeadLevel::SYSTEM ] << "Loading the global segment descriptor table." << endl;
        // 加载GDT
        this->load( );
        // 加载全局段中的TSS
        sout[ ostream::HeadLevel::SYSTEM ] << "Loading the task state segment in global segment." << endl;
        this->tss[ 0 ].load_tr( SELECTOR_TSS );
        Lib::IO::sout[ Lib::IO::ostream::HeadLevel::OK ] << "Initialize the global segment descriptor table management." << Lib::IO::endl;
    }
    auto GlobalSegmentDescriptorTable::load( VOID ) CONST->VOID {
        ASM( "lgdt %[GDTR]" ::[ GDTR ] "m"( this->xdtr ) );
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
    auto GlobalSegmentDescriptorTable::read( VOID ) CONST->Lib::Types::Ptr< GlobalSegmentDescriptor > {
        ASM( "sgdt %0" ::"m"( this->xdtr ) );
        return this->xdtr.descriptor;
    }

    auto InterruptDescriptor::make( IN CONST Lib::Types::uint64_t entry_point,
                                    IN CONST Lib::Types::uint16_t selector,
                                    IN CONST Lib::Types::uint8_t ist,
                                    IN CONST Lib::Types::uint8_t attributes )
        ->Lib::Types::L_Ref< InterruptDescriptor > {
        this->offset_low = ( entry_point & 0xffff );
        this->selector = selector;
        this->ist = ist;
        this->attribute = attributes;
        this->offset_middle = static_cast< decltype( this->offset_middle ) >( ( entry_point >> 16 ) & 0xffff );
        this->offset_high = static_cast< decltype( this->offset_high ) >( ( entry_point >> 32 ) & 0xffffffff );
        return *this;
    }
    auto GlobalSegmentDescriptor::make( IN Lib::Types::uint64_t base,
                                        IN Lib::Types::uint64_t limit,
                                        IN Lib::Types::uint64_t access )
        ->Lib::Types::L_Ref< GlobalSegmentDescriptor > {
        this->limit_low = limit & 0xffff;
        this->base_low = base & 0xffff;
        this->base_middle = ( base & 0xff0000 ) >> 16;
        this->access_right = access & 0xff;
        this->limit_high = ( ( limit >> 16 ) & 0xf ) | ( ( access >> 8 ) & 0xf0 );
        this->base_high = ( base >> 24 ) & 0xff;
        return *this;
    }
}
