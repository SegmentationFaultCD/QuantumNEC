#include <kernel/driver/sound/arch/x86_64/pcspeaker.hpp>
#include <lib/spin_lock.hpp>
#include <kernel/global/arch/x86_64/global.hpp>
#include <kernel/cpu/cpu.hpp>
PUBLIC namespace QuantumNEC::Kernel::x86_64 {
    PRIVATE Lib::SpinLock lock { };
    PCSpeaker::PCSpeaker( VOID ) noexcept {
    }
    PCSpeaker::~PCSpeaker( VOID ) noexcept {
    }
    auto PCSpeaker::enable_beep( VOID ) -> VOID {
        lock.acquire( );
        if ( !beeping ) {
            CPU::io_out8( SPEAKER_REG, CPU::io_in8( SPEAKER_REG ) | 0x03 );
            beeping = TRUE;
        }
        lock.release( );
    }

    auto PCSpeaker::disable_beep( VOID ) -> VOID {
        lock.acquire( );
        if ( beeping ) {
            CPU::io_out8( SPEAKER_REG, CPU::io_in8( SPEAKER_REG ) & 0xfc );
            beeping = FALSE;
        }
        lock.release( );
    }
}