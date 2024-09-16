#include <Boot/boot.hpp>
namespace QuantumNEC::Boot {
BootService::BootService( IN EFI_HANDLE ImageHandle, IN EFI_SYSTEM_TABLE *SystemTable ) noexcept {
    this->GlobalImageHandle = ImageHandle;
    this->GlobalSystemTable = SystemTable;
}

}     // namespace QuantumNEC::Boot