#include <kernel/display/print.hpp>
#include <kernel/driver/acpi/table.hpp>
#include <kernel/driver/acpi/xsdp.hpp>
#include <kernel/memory/allocator/page.hpp>
#include <kernel/memory/paging/hhdm.hpp>
#include <kernel/memory/paging/page_table.hpp>
namespace Driver {
auto initialize_acpi( limine_rsdp_response *rsdp ) -> void {
    auto xsdp = (XSDP *)Memory::physical_to_virtual( rsdp->address );
    using namespace Memory;

    Paging::kernel_page_table->map(
        std::uint64_t( Memory::virtual_to_physical( xsdp ) ),
        (std::uint64_t)xsdp,
        1,
        Paging::kernel_page_table->PAGE_PRESENT | Paging::kernel_page_table->PAGE_RW_W | Paging::kernel_page_table->PAGE_US_S,
        Page::Type::P4Kib );

    if ( xsdp->signature != XSDP::get_signature( ) ) {
    }
    auto xsdt = (XSDT *)Memory::physical_to_virtual( xsdp->get_xsdt( ) );
    Paging::kernel_page_table->map(
        std::uint64_t( Memory::virtual_to_physical( xsdt ) ),
        (std::uint64_t)xsdt,
        1,
        Paging::kernel_page_table->PAGE_PRESENT | Paging::kernel_page_table->PAGE_RW_W | Paging::kernel_page_table->PAGE_US_S,
        Page::Type::P4Kib );
    if ( xsdt->signature != XSDT::get_signature( ) ) {
    }
}
}     // namespace Driver