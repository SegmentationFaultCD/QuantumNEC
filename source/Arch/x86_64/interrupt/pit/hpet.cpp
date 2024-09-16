#include <Arch/x86_64/platform/platform.hpp>
PUBLIC namespace QuantumNEC::Architecture {
    Hpet ::Hpet( VOID ) noexcept {
        // 开启HPET
        auto info { reinterpret_cast< HpetInfo * >( Device::hpet->hpet_address.address ) };
        info->general_configuration = 3;
        CPUs::mfence( );
        // edge triggered & periodic
        info->timers[ 0 ].configuration_and_capability = 0x004c;
        CPUs::mfence( );
        // 1MS
        info->timers[ 0 ].comparator_value = 14318179;
        CPUs::mfence( );
        // init MAIN_CNT
        info->main_counter_value = 0;
        CPUs::mfence( );
    }
    Hpet::~Hpet( VOID ) noexcept {
    }
}