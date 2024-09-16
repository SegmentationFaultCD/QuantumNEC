#include <Boot/acpi.hpp>
#include <Guid/Acpi.h>
#include <IndustryStandard/Acpi.h>
#include <Boot/output.hpp>
namespace QuantumNEC::Boot {
AcpiService::AcpiService( VOID ) noexcept {
    if ( EFI_ERROR( EfiGetSystemConfigurationTable( &gEfiAcpi20TableGuid, &this->acpi_table ) ) ) {
#ifdef DEBUG
        print( u"[ERROR] Get System Configuration Table.\n" );

#endif
        while ( TRUE )
            ;
    }
#ifdef DEBUG
    print( u"[SUCCESS] Get System Configuration Table.\n" );

#endif
}
AcpiService::~AcpiService( VOID ) noexcept {
}
}     // namespace QuantumNEC::Boot