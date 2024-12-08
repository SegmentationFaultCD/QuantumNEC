#pragma once
#include <lib/Uefi.hpp>
namespace QuantumNEC::Kernel::x86_64 {
class SerialPort {
private:
    constexpr static auto COM1 = 0x3F8;
    constexpr static auto COM2 = 0x2F8;
    constexpr static auto COM3 = 0x3E8;
    constexpr static auto COM4 = 0x2E8;
    constexpr static auto COM5 = 0x5F8;
    constexpr static auto COM6 = 0x4F8;
    constexpr static auto COM7 = 0x5E8;
    constexpr static auto COM8 = 0x4E8;

public:
    explicit SerialPort( void ) noexcept;
    virtual ~SerialPort( void ) noexcept = default;

public:
    static auto read( void ) -> char_t;
    static auto write( char_t ch ) -> void;

private:
    inline static bool support_serial_port { false };
};
}     // namespace QuantumNEC::Kernel::x86_64