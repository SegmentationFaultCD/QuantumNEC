#include <Arch/x86_64/platform/platform.hpp>
#include <Kernel/print.hpp>
PUBLIC namespace QuantumNEC::Architecture {
    using namespace std;
    Device::Device( VOID ) noexcept :
        Graphics { }, Acpi { }, Sound { }, Time { }, FPU { }, Elf { } {
        // Device Management 初始化

        println< ostream::HeadLevel::OK >( "Initialize the Architecture Driver Management." );
    }
    Device::~Device( VOID ) noexcept {
    }
}