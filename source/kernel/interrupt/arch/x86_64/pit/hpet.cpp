#include <kernel/interrupt/arch/x86_64/pit/hpet.hpp>
#include <kernel/cpu/cpu.hpp>
#include <kernel/print.hpp>
#include <kernel/memory/memory.hpp>
#include <kernel/driver/driver.hpp>
#include <lib/spin_lock.hpp>
PUBLIC namespace QuantumNEC::Kernel::x86_64 {
    using namespace std;
    Hpet::Hpet( VOID ) noexcept {
        // 开启HPET
        auto info { reinterpret_cast< HpetInfo * >( Kernel::physical_to_virtual( Madt::hpet->hpet_address.address ) ) };
        info->general_configuration = 3;
        CPU::mfence( );
        // edge triggered & periodic
        info->timers[ 0 ].configuration_and_capability = 0x004c;
        CPU::mfence( );
        // 1MS
        info->timers[ 0 ].comparator_value = 14318179;
        CPU::mfence( );
        // init MAIN_CNT
        info->main_counter_value = 0;
        CPU::mfence( );

        println< ostream::HeadLevel::OK >( "Initialize the high precision event timer." );
    }
    Hpet::~Hpet( VOID ) noexcept {
    }
}
