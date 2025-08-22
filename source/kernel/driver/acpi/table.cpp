#include <kernel/display/print.hpp>
#include <kernel/driver/acpi/madt.hpp>
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
        Display::println( "XSDP signature {} can't satisfy!", xsdp->signature );
    }
    auto xsdt = (XSDT *)Memory::physical_to_virtual( xsdp->get_xsdt( ) );
    Paging::kernel_page_table->map(
        std::uint64_t( Memory::virtual_to_physical( xsdt ) ),
        (std::uint64_t)xsdt,
        1,
        Paging::kernel_page_table->PAGE_PRESENT | Paging::kernel_page_table->PAGE_RW_W | Paging::kernel_page_table->PAGE_US_S,
        Page::Type::P4Kib );
    if ( xsdt->signature != XSDT::get_signature( ) ) {
        Display::println( "XSDT signature {} can't satisfy!", xsdt->signature );
    }
    auto &madt = *xsdt->find_table< MADT >( );

    for ( auto length = madt.size( ), i = 0u; length > 0; ++i ) {
        using enum MADT::ICSAttribute;
        switch ( madt[ i ]->type ) {
        case PROCESSOR_LOCAL_APIC:
            
            break;
        case IO_APIC:
            break;
        case INTERRUPT_SOURCE_OVERRIDE:
            break;
        case NO_MASKABLE_INTERRUPT_SOURCE:
            break;
        case LOCAL_APIC_NO_MASKABLE_INTERRUPTS:
            break;
        case LOCAL_APIC_ADDRESS_OVERRIDE:
            break;
        case IO_SAPIC:
            break;
        case LOCAL_SAPIC:
            break;
        case PLATFORM_INTERRUPT_SOURCE:
            break;
        case PROCESSOR_LOCAL_x2APIC:
            break;
        case LOCAL_X2APIC_NMI:
            break;
        }
    }
}
}     // namespace Driver