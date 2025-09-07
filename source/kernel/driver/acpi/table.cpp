#include <kernel/display/print.hpp>
#include <kernel/driver/acpi/hpet_t.hpp>
#include <kernel/driver/acpi/madt.hpp>
#include <kernel/driver/acpi/table.hpp>
#include <kernel/driver/acpi/xsdp.hpp>
#include <kernel/driver/cpu/mmio.hpp>
#include <kernel/interrupt/apic.hpp>
#include <kernel/memory/allocator/page.hpp>
#include <kernel/memory/paging/hhdm.hpp>
#include <kernel/memory/paging/page_table.hpp>
namespace Driver {

auto initialize_acpi( limine_rsdp_response *rsdp ) -> void {
    auto xsdp = (XSDP *)Memory::physical_to_virtual( rsdp->address );
    using namespace Memory;

    paging->kernel_page_table->map(
        std::uint64_t( Memory::virtual_to_physical( xsdp ) ),
        (std::uint64_t)xsdp,
        1,
        paging->kernel_page_table->PAGE_PRESENT | paging->kernel_page_table->PAGE_RW_W | paging->kernel_page_table->PAGE_US_S,
        Page::Type::P4Kib );

    if ( xsdp->signature != XSDP::get_signature( ) ) {
        Display::println( "XSDP signature {} can't satisfy!", xsdp->signature );
    }
    auto xsdt = (XSDT *)Memory::physical_to_virtual( xsdp->get_xsdt( ) );
    paging->kernel_page_table->map(
        Memory::virtual_to_physical( xsdt ),
        (std::uint64_t)xsdt,
        1,
        paging->kernel_page_table->PAGE_PRESENT | paging->kernel_page_table->PAGE_RW_W | paging->kernel_page_table->PAGE_US_S,
        Page::Type::P2Mib );

    if ( xsdt->signature != XSDT::get_signature( ) ) {
        Display::println( "XSDT signature {} can't satisfy!", xsdt->signature );
    }

    std::construct_at( xsdt->find_table< MADT >( ) );
    std::construct_at( xsdt->find_table< HPET >( ) );

    // madt就跟在xsdt后面，不要map了
}
}     // namespace Driver