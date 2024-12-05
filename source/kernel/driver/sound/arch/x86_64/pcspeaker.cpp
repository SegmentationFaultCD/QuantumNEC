#include <kernel/cpu/cpu.hpp>
#include <kernel/driver/sound/arch/x86_64/pcspeaker.hpp>
#include <kernel/global/arch/x86_64/global.hpp>
#include <kernel/interrupt/arch/x86_64/pit/hpet.hpp>
#include <lib/spin_lock.hpp>
PUBLIC namespace QuantumNEC::Kernel::x86_64 {
    PRIVATE Lib::SpinLock lock { };
    PCSpeaker::PCSpeaker( VOID ) noexcept {
    }
    PCSpeaker::~PCSpeaker( VOID ) noexcept {
    }
    auto PCSpeaker::enable( VOID ) -> VOID {
        lock.acquire( );
        constexpr auto div = 1193180 / 1000;
        CPU::io_out8( 0x43, 0xb6 );
        CPU::io_out8( 0x42, (uint8_t)div );
        CPU::io_out8( 0x42, (uint8_t)( div >> 8ul ) );
        auto tmp = CPU::io_in8( SPEAKER_REG );
        if ( tmp != ( tmp | 3 ) ) {
            CPU::io_out8( SPEAKER_REG, tmp | 3 );
        }
        lock.release( );
    }

    auto PCSpeaker::disable( VOID ) -> VOID {
        lock.acquire( );
        CPU::io_out8( SPEAKER_REG, CPU::io_in8( SPEAKER_REG ) & 0xfc );
        lock.release( );
    }
    auto PCSpeaker::beep( VOID ) -> VOID {
        enable( );
        Hpet::sleep( 10 );
        disable( );
    }
}