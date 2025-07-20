#include <kernel/display/print.hpp>
#include <kernel/driver/serial_port/serial_port.hpp>
#include <kernel/interrupt/idt.hpp>
#include <kernel/memory/segment/gdt.hpp>
#include <lib/bitset.hpp>
#include <lib/string.hpp>
#include <limine.h>
namespace {

__attribute__( ( used, section( ".requests" ) ) ) volatile LIMINE_BASE_REVISION( 2 );
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

extern "C" auto loader_entry( void ) -> void {
    Display::initialize( framebuffer_request.response->framebuffers[ 0 ] );
    Driver::SerialPort::initialize( );
    Interrupt::IDT::initialize( 0 );
    Memory::GDT::initialize( 0 );

    while ( true );
}