#include <kernel/display/print.hpp>
#include <kernel/driver/serial_port/serial_port.hpp>
#include <kernel/interrupt/idt.hpp>
#include <kernel/memory/allocator/page.hpp>
#include <kernel/memory/segment/gdt.hpp>
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
    .id       = LIMINE_FRAMEBUFFER_REQUEST,
    .revision = 3,
    .response = nullptr
};
__attribute__( ( used, section( ".requests" ) ) ) volatile limine_memmap_request memmap_request = {
    .id       = LIMINE_MEMMAP_REQUEST,
    .revision = 3,
    .response = nullptr
};
__attribute__( ( used, section( ".requests" ) ) ) volatile limine_rsdp_request acpi_request = {
    .id       = LIMINE_RSDP_REQUEST,
    .revision = 3,
    .response = nullptr
};
__attribute__( ( used, section( ".requests" ) ) ) volatile limine_hhdm_request hhdm_request = {
    .id       = LIMINE_HHDM_REQUEST,
    .revision = 3,
    .response = nullptr
};
__attribute__( ( used, section( ".requests" ) ) ) volatile limine_smp_request smp_request = {
    .id       = LIMINE_SMP_REQUEST,
    .revision = 3,
    .response = nullptr
};
__attribute__( ( used, section( ".requests" ) ) ) volatile limine_paging_mode_request paging_mode_request = {
    .id       = LIMINE_PAGING_MODE_REQUEST,
    .revision = 3,
    .response = nullptr
};

__attribute__( ( used, section( ".requests" ) ) ) volatile limine_module_request modules_request = {
    .id       = LIMINE_MODULE_REQUEST,
    .revision = 3,
    .response = nullptr
};

__attribute__( ( used, section( ".limine_requests" ) ) ) static volatile struct limine_stack_size_request stack_size_request = {
    .id         = LIMINE_STACK_SIZE_REQUEST,
    .revision   = 3,
    .stack_size = 4096,
};
}     // namespace

// Finally, define the start and end markers for the Limine requests.
// These can also be moved anywhere, to any .cpp file, as seen fit.

namespace {

__attribute__( ( used, section( ".requests_start_marker" ) ) ) volatile LIMINE_REQUESTS_START_MARKER;

__attribute__( ( used, section( ".requests_end_marker" ) ) ) volatile LIMINE_REQUESTS_END_MARKER;

}     // namespace

#include <kernel/memory/allocator/kheap.hpp>
#include <kernel/memory/paging/hhdm.hpp>
#include <kernel/memory/paging/page_table.hpp>
#include <lib/format.hpp>
#include <ranges>
#include <span>
#include <vector>
auto ff( std::span< int > f ) {
    char buf[ 114 ];
    Library::utoa( std::uint64_t( f[ 0 ] ), buf, 16 );
    Driver::SerialPort { }.print( buf );
    Driver::SerialPort { }.print( "\n" );
}
extern "C" auto loader_entry( void ) -> void {
    Display::initialize( framebuffer_request.response->framebuffers[ 0 ] );
    Driver::SerialPort::initialize( );
    Interrupt::IDT::initialize( 0 );
    Memory::GDT::initialize( 0 );
    Memory::Page::page_memory_initialize( memmap_request.response );
    Memory::hhdm_initialize( hhdm_request.response );
    Memory::KernelHeap::kernel_heap_initialize( );
    Memory::Paging::initialize( paging_mode_request.response );

    using namespace Memory::Page;

    char                                   buf[ 114 ];
    Memory::Page::allocator< Type::P2Mib > a;
    auto                                   p = a.allocate( 1025 );
    Library::utoa( std::uint64_t( p ), buf, 16 );
    Driver::SerialPort { }.print( buf );
    Driver::SerialPort { }.print( "\n" );
    auto p2 = a.allocate( 1026 );
    Library::utoa( std::uint64_t( p2 ), buf, 16 );
    Driver::SerialPort { }.print( buf );
    Driver::SerialPort { }.print( "\n" );
    auto p3 = a.allocate( 2 );
    Library::utoa( std::uint64_t( p3 ), buf, 16 );
    Driver::SerialPort { }.print( buf );
    Driver::SerialPort { }.print( "\n" );
    auto p4 = a.allocate( 1 );
    Library::utoa( std::uint64_t( p4 ), buf, 16 );
    Driver::SerialPort { }.print( buf );
    Driver::SerialPort { }.print( "\n" );
    a.deallocate( p, 1025 );
    p = a.allocate( 1025 );
    Library::utoa( std::uint64_t( p ), buf, 16 );
    Driver::SerialPort { }.print( buf );
    Driver::SerialPort { }.print( "\n" );
    auto p6 = a.allocate( 1025 );
    Library::utoa( std::uint64_t( p6 ), buf, 16 );
    Driver::SerialPort { }.print( buf );
    Driver::SerialPort { }.print( "\n" );
    a.deallocate( p6, 1025 );
    auto p7 = a.allocate( 1025 );
    Library::utoa( std::uint64_t( p7 ), buf, 16 );
    Driver::SerialPort { }.print( buf );
    Driver::SerialPort { }.print( "\n" );
    Memory::Page::allocator< Type::P4Kib > b;
    auto                                   t1 = b.allocate( 114 );
    Library::utoa( std::uint64_t( t1 ), buf, 16 );
    Driver::SerialPort { }.print( buf );
    Driver::SerialPort { }.print( "\n" );
    auto t2 = b.allocate( 1112 );
    Library::utoa( std::uint64_t( t2 ), buf, 16 );
    Driver::SerialPort { }.print( buf );
    Driver::SerialPort { }.print( "\n" );
    auto t3 = b.allocate( 12 );
    Library::utoa( std::uint64_t( t3 ), buf, 16 );
    Driver::SerialPort { }.print( buf );
    Driver::SerialPort { }.print( "\n" );
    Memory::Page::allocator< Type::P1Gib > c;

    auto c1 = c.allocate( 514 );
    Library::utoa( std::uint64_t( c1 ), buf, 16 );
    Driver::SerialPort { }.print( buf );
    Driver::SerialPort { }.print( "\n" );
    auto c2 = c.allocate( 1321 );
    Library::utoa( std::uint64_t( c2 ), buf, 16 );
    Driver::SerialPort { }.print( buf );
    Driver::SerialPort { }.print( "\n" );
    auto c3 = c.allocate( 1 );
    Library::utoa( std::uint64_t( c3 ), buf, 16 );
    Driver::SerialPort { }.print( buf );
    Driver::SerialPort { }.print( "\n" );

    // C++ standard library test

    std::basic_string< char, std::char_traits< char >, Memory::KernelHeap::allocator< char > > string;     // 空
                                                                                                           //
    string.append( "const char *s" );
    Driver::SerialPort { }.print( string.c_str( ) );

    Driver::SerialPort { }.print( buf );
    Driver::SerialPort { }.print( "\n" );
    int s[] = { 1, 2, 3, 4 };
    ff( s );

    std::vector< int, Memory::KernelHeap::allocator< int > > vec;
    vec.emplace_back( 1 );

    vec.push_back( 2 );

    for ( auto i : vec ) {
        Library::utoa( std::uint64_t( i ), buf, 16 );
        Driver::SerialPort { }.print( buf );
        Driver::SerialPort { }.print( "\n" );
    }

    auto s2 = string | std::ranges::views::filter( []( const char c ) -> bool { return c != ' '; } ) | std::ranges::to< std::basic_string< char, std::char_traits< char >, Memory::KernelHeap::allocator< char > > >( );
    Driver::SerialPort { }.print( s2.c_str( ) );
    Driver::SerialPort { }.print( "\n" );
    char buff[ 1145 ];

    Driver::SerialPort { }.print( Library::format( "{1:*>18x} {0:*>18x}", 1145ul, 19198ul ).c_str( ) );
    Driver::SerialPort { }.print( "\n" );

    while ( true );
}
