#include <kernel/cpu/cpu.hpp>
#include <kernel/driver/sound/x86_64/pcspeaker.hpp>
#include <kernel/global/x86_64/global.hpp>
#include <kernel/interrupt/x86_64/pit/hpet.hpp>
namespace QuantumNEC::Kernel::x86_64 {

PCSpeaker::PCSpeaker( void ) noexcept {
}
PCSpeaker::~PCSpeaker( void ) noexcept {
}
auto PCSpeaker::enable( void ) -> void {
    constexpr auto div = 1193180 / 1000;
    CPU::io_out8( 0x43, 0xb6 );
    CPU::io_out8( 0x42, (uint8_t)div );
    CPU::io_out8( 0x42, (uint8_t)( div >> 8ul ) );
    auto tmp = CPU::io_in8( SPEAKER_REG );
    if ( tmp != ( tmp | 3 ) ) {
        CPU::io_out8( SPEAKER_REG, tmp | 3 );
    }
}

auto PCSpeaker::disable( void ) -> void {
    CPU::io_out8( SPEAKER_REG, CPU::io_in8( SPEAKER_REG ) & 0xfc );
}
auto PCSpeaker::beep( void ) -> void {
    enable( );
    Hpet::sleep( 10 );
    disable( );
}
}     // namespace QuantumNEC::Kernel::x86_64