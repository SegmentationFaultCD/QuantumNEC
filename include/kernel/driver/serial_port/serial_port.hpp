#pragma once
namespace Driver {

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
    ~SerialPort( void ) noexcept = default;

public:
    auto read( void ) -> char;
    auto write( char ch ) -> void;
    auto print( const char *str ) {
        while ( *str ) {
            this->write( *str );
            str++;
        }
    }

public:
    static auto initialize( void ) -> void;

private:
    inline static bool support_serial_port { false };
};

}     // namespace Driver