#include <kernel/cpu/cpu.hpp>
#include <kernel/driver/driver.hpp>
#include <kernel/interrupt/x86_64/pit/hpet.hpp>
#include <kernel/memory/memory.hpp>
#include <kernel/print.hpp>
#include <lib/spin_lock.hpp>
namespace QuantumNEC::Kernel::x86_64 {
using namespace std;
Hpet::Hpet( void ) noexcept {
    this->hpet_info = reinterpret_cast< HpetInfo * >( Kernel::x86_64::physical_to_virtual( Madt::hpet->hpet_address.address ) );

    // edge triggered & periodic
    this->hpet_info->timers[ 0 ].configuration_and_capability = 0x004c;
    CPU::mfence( );
    // 1MS
    this->hpet_info->timers[ 0 ].comparator_value = 14318179;
    CPU::mfence( );
    // init MAIN_CNT
    this->hpet_info->main_counter_value = 0;
    CPU::mfence( );
    // 开启HPET
    this->hpet_info->general_configuration = 3;
    CPU::mfence( );
    this->periods = this->hpet_info->general_capabilities >> 32;
    this->periods /= 1000000;
}
Hpet::~Hpet( void ) noexcept {
}

auto Hpet::sleep( uint64_t ns ) -> void {
    auto total_ns = hpet_info->main_counter_value * periods + ns;
    while ( hpet_info->main_counter_value * periods < total_ns );
}
}     // namespace QuantumNEC::Kernel::x86_64
