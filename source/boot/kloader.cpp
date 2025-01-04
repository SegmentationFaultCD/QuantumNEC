#include <limine.h>

#include <lib/Uefi.hpp>

#include <kernel/task/general/pcb/pcb.hpp>
using namespace QuantumNEC;

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
    .stack_size = Kernel::TASK_KERNEL_STACK_SIZE,
};
}     // namespace

// Finally, define the start and end markers for the Limine requests.
// These can also be moved anywhere, to any .cpp file, as seen fit.

namespace {

__attribute__( ( used, section( ".requests_start_marker" ) ) ) volatile LIMINE_REQUESTS_START_MARKER;

__attribute__( ( used, section( ".requests_end_marker" ) ) ) volatile LIMINE_REQUESTS_END_MARKER;

}     // namespace

auto micro_kernel_entry( IN BootConfig &config ) -> void;

_C_LINK auto loader_entry( void ) -> void {
    BootConfig config { };
    config.graphics_data = *framebuffer_request.response->framebuffers[ 0 ];
    config.memory_map    = *memmap_request.response;
    config.acpi_table    = *acpi_request.response;
    config.hhdm          = *hhdm_request.response;
    config.smp           = *smp_request.response;
    config.paging_mode   = *paging_mode_request.response;
    config.modules       = *modules_request.response;
    micro_kernel_entry( config );
    while ( true );
}