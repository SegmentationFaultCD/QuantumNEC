#include <Arch/x86_64/platform/platform.hpp>
#include <Lib/IO/Stream/iostream>
PUBLIC namespace QuantumNEC::Architecture {
    Device::Device( IN Lib::Types::Ptr< Lib::Types::BootConfig > _config ) noexcept :
        Graphics { _config }, Acpi { _config }, Beep { }, CMOS { }, FPU { } {
        // Device Management 初始化
        Lib::IO::sout[ Lib::IO::ostream::HeadLevel::OK ] << "Initialize the Architecture Driver Management." << Lib::IO::endl;
    }
    Device::~Device( VOID ) noexcept {
    }
}