#include <Arch/x86_64/platform/platform.hpp>
PUBLIC namespace QuantumNEC::Architecture {
    Beep::Beep( VOID ) noexcept {
    }
    Beep::~Beep( VOID ) noexcept {
    }
    auto Beep::enable_beep( VOID )->VOID {
        if ( !beeping ) {
            CPUs::io_out8( SPEAKER_REG, CPUs::io_in8( SPEAKER_REG ) | 0x03 );
            beeping = TRUE;
        }
    }

    auto Beep::disable_beep( VOID )->VOID {
        if ( beeping ) {
            CPUs::io_out8( SPEAKER_REG, CPUs::io_in8( SPEAKER_REG ) & 0xfc );
            beeping = FALSE;
        }
    }
}