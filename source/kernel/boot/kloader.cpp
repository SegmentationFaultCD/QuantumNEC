#include <kernel/display/print.hpp>
#include <kernel/driver/cpu/smp.hpp>
#include <kernel/driver/serial_port/serial_port.hpp>
#include <kernel/interrupt/idt.hpp>
#include <kernel/memory/allocator/page.hpp>
#include <kernel/memory/segment/gdt.hpp>
#include <kernel/task/schedule/scheduler.hpp>
#include <lib/bitset.hpp>
#include <lib/string.hpp>
#include <limine.h>
namespace {

__attribute__( ( used, section( ".requests" ) ) ) volatile LIMINE_BASE_REVISION( 3 );
}

// The Limine requests can be placed anywhere, but it is important that
// the compiler does not optimise them away, so, usually, they should
// be made volatile or equivalent, _and_ they should be accessed at least
// once or marked as used with the "used" attribute as done here.

namespace {

__attribute__( ( used, section( ".requests" ) ) ) volatile limine_framebuffer_request framebuffer_request = {
    .id = LIMINE_FRAMEBUFFER_REQUEST,
    .revision = 3,
    .response = nullptr
};
__attribute__( ( used, section( ".requests" ) ) ) volatile limine_memmap_request memmap_request = {
    .id = LIMINE_MEMMAP_REQUEST,
    .revision = 3,
    .response = nullptr
};
__attribute__( ( used, section( ".requests" ) ) ) volatile limine_rsdp_request acpi_request = {
    .id = LIMINE_RSDP_REQUEST,
    .revision = 3,
    .response = nullptr
};
__attribute__( ( used, section( ".requests" ) ) ) volatile limine_hhdm_request hhdm_request = {
    .id = LIMINE_HHDM_REQUEST,
    .revision = 3,
    .response = nullptr
};
__attribute__( ( used, section( ".requests" ) ) ) volatile limine_smp_request smp_request = {
    .id = LIMINE_SMP_REQUEST,
    .revision = 3,
    .response = nullptr
};
__attribute__( ( used, section( ".requests" ) ) ) volatile limine_paging_mode_request paging_mode_request = {
    .id = LIMINE_PAGING_MODE_REQUEST,
    .revision = 3,
    .response = nullptr
};

__attribute__( ( used, section( ".requests" ) ) ) volatile limine_module_request modules_request = {
    .id = LIMINE_MODULE_REQUEST,
    .revision = 3,
    .response = nullptr
};

__attribute__( ( used, section( ".limine_requests" ) ) ) static volatile struct limine_stack_size_request stack_size_request = {
    .id = LIMINE_STACK_SIZE_REQUEST,
    .revision = 3,
    .stack_size = 1024 * 64,
};
}     // namespace

// Finally, define the start and end markers for the Limine requests.
// These can also be moved anywhere, to any .cpp file, as seen fit.

namespace {

__attribute__( ( used, section( ".requests_start_marker" ) ) ) volatile LIMINE_REQUESTS_START_MARKER;

__attribute__( ( used, section( ".requests_end_marker" ) ) ) volatile LIMINE_REQUESTS_END_MARKER;

}     // namespace

#include <kernel/display/print.hpp>
#include <kernel/driver/cpu/sse.hpp>
#include <kernel/memory/allocator/kheap.hpp>
#include <kernel/memory/paging/hhdm.hpp>
#include <kernel/memory/paging/page_table.hpp>
#include <lib/format.hpp>
#define TERMINAL_EMBEDDED_FONT
#include <kernel/display/print.hpp>
#include <kernel/driver/acpi/table.hpp>
#include <kernel/interrupt/apic.hpp>
#include <kernel/syscall/syscall.hpp>
#include <kernel/task/task.hpp>

#include <kernel/syscall/module_loader/loader.hpp>
#include <os_terminal.h>
#include <ranges>
#include <span>
#include <vector>
auto ff( std::span< int > f ) {
    char buf[ 114 ];
    Library::utoa( std::uint64_t( f[ 0 ] ), buf, 16 );
    Driver::SerialPort { }.print( buf );
    Driver::SerialPort { }.print( "\n" );
}

auto alloc( std::size_t size ) -> void * {
    return operator new( size );
}
auto free( void *address ) -> void {
    operator delete( address );
}

extern "C" [[noreturn]] auto loader_entry( void ) -> void {
    Driver::initialize_sse( );

    Driver::serial_port.initialize( );
    Interrupt::idt = Interrupt::idt->initialize( 0 );
    Memory::gdt = Memory::gdt->initialize( 0 );
    Memory::Page::initialize( memmap_request.response );
    Memory::hhdm_initialize( hhdm_request.response );
    Memory::KernelHeap::initialize( );
    Memory::paging = Memory::paging->initialize( paging_mode_request.response );
    Display::initialize( framebuffer_request.response->framebuffers[ 0 ] );
    Driver::initialize_acpi( acpi_request.response );
    Interrupt::apic.initialize( true );
    Task::scheduler->initialize( );
    Task::initialize_task( 0 );
    Kernel::syscall = Kernel::syscall->initialize( );
    Driver::initialize_smp( smp_request.response );
    Kernel::module_loader = Kernel::module_loader->initialize( modules_request.response );

    Interrupt::idt->enable_interrupt( );
    while ( true );
    using namespace Memory::Page;

    Memory::Page::allocator< Type::P2Mib > a;
    auto p = a.allocate( 1025 );
    Display::println( "{}", p );

    auto p2 = a.allocate( 1026 );
    Display::println( "{}", p2 );
    auto p3 = a.allocate( 2 );
    Display::println( "{}", p3 );
    auto p4 = a.allocate( 1 );
    Display::println( "{}", p4 );
    a.deallocate( p, 1025 );
    p = a.allocate( 1025 );
    Display::println( "{}", p );
    auto p6 = a.allocate( 1025 );
    Display::println( "{}", p6 );
    a.deallocate( p6, 1025 );
    auto p7 = a.allocate( 1025 );
    Display::println( "{}", p7 );
    Memory::Page::allocator< Type::P4Kib > b;
    auto t1 = b.allocate( 114 );
    Display::println( "{}", t1 );
    auto t2 = b.allocate( 1112 );
    Display::println( "{}", t2 );
    auto t3 = b.allocate( 12 );
    Display::println( "{}", t3 );

    Memory::Page::allocator< Type::P1Gib > c;

    auto c1 = c.allocate( 514 );
    Display::println( "{}", c1 );
    auto c2 = c.allocate( 1321 );
    Display::println( "{}", c2 );
    auto c3 = c.allocate( 1 );
    Display::println( "{}", c3 );

    // C++ standard library test

    std::basic_string< char, std::char_traits< char >, Memory::KernelHeap::allocator< char > > string;     // 空
                                                                                                           //
    string.append( "const char *s" );
    Display::println( "{}", string.c_str( ) );
    int s[] = { 1, 2, 3, 4 };
    ff( s );

    std::vector< int, Memory::KernelHeap::allocator< int > > vec { 3 };
    vec.clear( );

    Display::println( "{}", vec[ 0 ] );

    auto s2 = string | std::ranges::views::filter( []( const char c ) -> bool { return c != ' '; } ) | std::ranges::to< std::basic_string< char, std::char_traits< char >, Memory::KernelHeap::allocator< char > > >( );
    Display::println( "{}", s2.c_str( ) );
    void *s1111 = (void *)111212121l;
    Display::println( "{} {:x} {:x} {:x} {:x} {:x}", s1111, 114514, 1, 1, 1, 1 );

    // TerminalDisplay td;
    // td.red_mask_size    = framebuffer_request.response->framebuffers[ 0 ]->red_mask_size;
    // td.blue_mask_size   = framebuffer_request.response->framebuffers[ 0 ]->blue_mask_size;
    // td.green_mask_size  = framebuffer_request.response->framebuffers[ 0 ]->green_mask_size;
    // td.red_mask_shift   = framebuffer_request.response->framebuffers[ 0 ]->red_mask_shift;
    // td.blue_mask_shift  = framebuffer_request.response->framebuffers[ 0 ]->blue_mask_shift;
    // td.green_mask_shift = framebuffer_request.response->framebuffers[ 0 ]->green_mask_shift;
    // td.buffer           = (uint32_t *)framebuffer_request.response->framebuffers[ 0 ]->address;
    // td.height           = framebuffer_request.response->framebuffers[ 0 ]->height;
    // td.width            = framebuffer_request.response->framebuffers[ 0 ]->width;
    // td.pitch            = framebuffer_request.response->framebuffers[ 0 ]->pitch;

    // terminal_init( &td, 15.0f, alloc, free );
    while ( true );
    // terminal_process( "Hello world" );
}
