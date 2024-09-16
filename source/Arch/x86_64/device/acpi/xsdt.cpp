#include <Arch/x86_64/platform/platform.hpp>
#include <Kernel/print.hpp>
PUBLIC namespace QuantumNEC::Architecture {
    using namespace Kernel;
    using namespace Lib;
    Xsdt::Xsdt( IN Xsdp & xsdp ) noexcept {
        this->xsdt = (XSDT *)uint64_t( xsdp.get_rsdp( )->xsdt_address );
        if ( !this->xsdt || this->xsdt->signature != this->signature || !xsdp.check_sum( this->xsdt, this->xsdt->length ) ) {
            println< ostream::HeadLevel::ERROR >( "XSDT is not valid." );
            while ( true );
        }
    }
}