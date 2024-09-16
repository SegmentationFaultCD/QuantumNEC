#pragma once
#include <Boot/base.hpp>
namespace QuantumNEC::Boot {
class AcpiService
{
public:
    explicit AcpiService( VOID ) noexcept;
    ~AcpiService( VOID ) noexcept;

private:
    VOID *acpi_table;
};

}     // namespace QuantumNEC::Boot