#include <kernel/cpu/cpu.hpp>
#include <kernel/interrupt/x86_64/entry/idt.hpp>
#include <kernel/print.hpp>
#include <lib/spin_lock.hpp>
#include <libcxx/cstring.hpp>
using namespace QuantumNEC::Kernel::x86_64;
using namespace std;
/**
 * @brief 设置中断入口（Trap）
 */
#define SET_TRAP_HANDLER( index, ist )                \
    ASM( "LEAQ InterruptHandler" #index "(%%RIP), %0" \
         : "=r"( function ) );                        \
    this->idt->descriptor_table[ 0 ][ index ].make(   \
        function,                                     \
        SELECTOR_CODE64_KERNEL,                       \
        ist,                                          \
        static_cast< uint8_t >( InterruptDescriptorAttribute::TRAP ) )
/**
 * @brief 设置中断入口（System）
 */
#define SET_SYSTEM_HANDLER( index, ist )              \
    ASM( "LEAQ InterruptHandler" #index "(%%RIP), %0" \
         : "=r"( function ) );                        \
    this->idt->descriptor_table[ 0 ][ index ].make(   \
        function,                                     \
        SELECTOR_CODE64_KERNEL,                       \
        ist,                                          \
        static_cast< uint8_t >( InterruptDescriptorAttribute::SYSTEM ) )
/**
 * @brief 设置中断入口（System——Interrupt）
 */
#define SET_SYSTEM_INTERRUPT_HANDLER( index, ist )    \
    ASM( "LEAQ InterruptHandler" #index "(%%RIP), %0" \
         : "=r"( function ) );                        \
    this->idt->descriptor_table[ 0 ][ index ].make(   \
        function,                                     \
        SELECTOR_CODE64_KERNEL,                       \
        ist,                                          \
        static_cast< uint8_t >( InterruptDescriptorAttribute::SYSTEM_INTERRUPT ) )
/**
 * @brief 设置中断入口（Interrupt）
 */
#define SET_INTERRUPT_HANDLER( index, ist )           \
    ASM( "LEAQ InterruptHandler" #index "(%%RIP), %0" \
         : "=r"( function ) );                        \
    this->idt->descriptor_table[ 0 ][ index ].make(   \
        function,                                     \
        SELECTOR_CODE64_KERNEL,                       \
        ist,                                          \
        static_cast< uint8_t >( InterruptDescriptorAttribute::INTERRUPT ) )

inline static InterruptDescriptorTable::_IDT _idt { };
InterruptDescriptorTable::InterruptDescriptorTable( void ) noexcept {
    this->idt = new ( &_idt ) _IDT { };
    CPU::cli( );     // 关中断
    memset( this->idt->xdtr->descriptor, 0, INTERRUPT_DESCRIPTOR_COUNT * sizeof( InterruptDescriptor ) );
    // for ( uint16_t i { }; i < INTERRUPT_DESCRIPTOR_COUNT; ++i ) {
    //     interrupt_name[ i ] = "unknown";     // 先统一赋值为unknown
    // }
    // interrupt_name[ 0 ]  = "#DE Divide-by-zero";
    // interrupt_name[ 1 ]  = "#DB Debug";
    // interrupt_name[ 2 ]  = "--- Non Maskable Interrupt(NMI)";
    // interrupt_name[ 3 ]  = "#BP Breakpoint";
    // interrupt_name[ 4 ]  = "#OF Overflow";
    // interrupt_name[ 5 ]  = "#BR Bound Range Exceeded";
    // interrupt_name[ 6 ]  = "#UD Invalid Opcode";
    // interrupt_name[ 7 ]  = "#NM Device Not Available";
    // interrupt_name[ 8 ]  = "#DF Double Fault";
    // interrupt_name[ 9 ]  = "--- Coprocessor Segment Overrun";
    // interrupt_name[ 10 ] = "#TS Invalid TSS";
    // interrupt_name[ 11 ] = "#NP Segment Not Present";
    // interrupt_name[ 12 ] = "#SS Stack-Segment Fault";
    // interrupt_name[ 13 ] = "#GP General Protection Fault";
    // interrupt_name[ 14 ] = "#PF Page Fault";
    // interrupt_name[ 15 ] = "--- Intel Reserved";     // 第15项是intel保留项，未使用
    // interrupt_name[ 16 ] = "#MF x87 Floating-Point Exception";
    // interrupt_name[ 17 ] = "#AC Alignment Check";
    // interrupt_name[ 18 ] = "#MC Machine Check";
    // interrupt_name[ 19 ] = "#XM/#XF SIMD Floating-Point Exception";
    // interrupt_name[ 20 ] = "#VE Virtualization Exception";
    // interrupt_name[ 21 ] = "#CP Control Protection Exception";
    // for ( uint16_t i { 22 }; i <= 27; ++i ) {
    //     interrupt_name[ i ] = "--- System Reserved";
    // }
    // interrupt_name[ 28 ] = "#HV Exception Injected By The Virtual Machine";
    // interrupt_name[ 29 ] = "#VC VMM Communication Failed";
    // interrupt_name[ 30 ] = "#SX Security Exception";
    // interrupt_name[ 31 ] = "--- System Reserved";

    /* 注册所有入口函数到中断描述符表 */
    uint64_t function { };
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
    println< print_level::SYSTEM >( "Loading the interrupt descriptor table." );

    this->idt->load( 0 );

    println< print_level::OK >( "Initialize the interrupt descriptor table management." );
}

auto InterruptDescriptorTable::_IDT::load( [[maybe_unused]] IN uint64_t processor_id ) const -> void {
    ASM( "lidt %0" ::"m"( *this->xdtr ) );
    return;
}
auto InterruptDescriptorTable::_IDT::read( [[maybe_unused]] IN uint64_t processor_id ) const -> InterruptDescriptor * {
    // 获取idt地址并返回
    ASM( "sidt %0" ::"m"( *this->xdtr ) );
    return this->xdtr->descriptor;
}

auto InterruptDescriptor::make( IN uint64_t entry_point,
                                IN uint16_t selector,
                                IN uint8_t  ist,
                                IN uint8_t  attributes )
    -> InterruptDescriptor & {
    this->offset_low    = ( entry_point & 0xffff );
    this->selector      = selector;
    this->ist           = ist;
    this->attribute     = attributes;
    this->offset_middle = static_cast< decltype( this->offset_middle ) >( ( entry_point >> 16 ) & 0xffff );
    this->offset_high   = static_cast< decltype( this->offset_high ) >( ( entry_point >> 32 ) & 0xffffffff );
    return *this;
}