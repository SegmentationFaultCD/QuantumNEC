#include <kernel/driver/acpi/hpet_t.hpp>
#include <kernel/interrupt/hpet.hpp>
#include <kernel/memory/paging/hhdm.hpp>
#include <kernel/memory/paging/page_table.hpp>
namespace Driver {
HPET::HPET( void ) noexcept {
    using namespace Memory;
    Paging::kernel_page_table->map( this->base_address.address,
                                    physical_to_virtual( this->base_address.address ),
                                    1,
                                    Paging::kernel_page_table->PAGE_PRESENT | Paging::kernel_page_table->PAGE_RW_W | Paging::kernel_page_table->PAGE_US_S,
                                    Page::Type::P4Kib );
    Interrupt::hpet = (decltype( Interrupt::hpet ))physical_to_virtual( this->base_address.address );

    std::construct_at( Interrupt::hpet );
}

}     // namespace Driver