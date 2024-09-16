#include <Arch/x86_64/platform/platform.hpp>
#include <Lib/spin_lock.hpp>
PUBLIC namespace QuantumNEC::Architecture {
    PRIVATE Lib::SpinLock lock { };
    PCSpeaker::PCSpeaker( VOID ) noexcept {
    }
    PCSpeaker::~PCSpeaker( VOID ) noexcept {
    }
    auto PCSpeaker::enable_beep( VOID ) -> VOID {
        lock.acquire( );
        if ( !beeping ) {
            CPUs::io_out8( SPEAKER_REG, CPUs::io_in8( SPEAKER_REG ) | 0x03 );
            beeping = TRUE;
        }
        lock.release( );
    }

    auto PCSpeaker::disable_beep( VOID ) -> VOID {
        lock.acquire( );
        if ( beeping ) {
            CPUs::io_out8( SPEAKER_REG, CPUs::io_in8( SPEAKER_REG ) & 0xfc );
            beeping = FALSE;
        }
        lock.release( );
    }
}