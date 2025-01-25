#include <kernel/cpu/cpu.hpp>
#include <kernel/driver/driver.hpp>
#include <kernel/interrupt/x86_64/pit/apic_timer.hpp>
#include <kernel/interrupt/x86_64/pit/hpet.hpp>
#include <kernel/memory/memory.hpp>

namespace QuantumNEC::Kernel::x86_64 {
using namespace std;
Hpet::Hpet( void ) noexcept {
    this->hpet_info = reinterpret_cast< HpetInfo * >( Kernel::x86_64::physical_to_virtual( Madt::hpet->hpet_address.address ) );

    // init MAIN_CNT
    this->hpet_info->main_counter_value = 0;
    CPU::mfence( );
    // edge triggered & periodic
    this->hpet_info->timers[ 0 ].configuration_and_capability = 0x4c;
    CPU::mfence( );
    // 1MS
    this->hpet_info->timers[ 0 ].comparator_value = 14318179;
    CPU::mfence( );

    this->periods                 = ( this->hpet_info->general_capabilities >> 32 ) & 0xffffffff;
    this->the_number_of_timer_cap = ( this->hpet_info->general_capabilities >> 8 ) & 0x1f;     // 读取计时器数量

    this->enable_hpet( );
}
Hpet::~Hpet( void ) noexcept {
}
auto Hpet::enable_hpet( void ) -> void {
    hpet_info->general_configuration = 3;
    CPU::mfence( );
}
auto Hpet::disable_hpet( void ) -> void {
    hpet_info->general_configuration = 0;
    CPU::mfence( );
}
auto Hpet::sleep( uint64_t ns ) -> void {
    auto total_ns = hpet_info->main_counter_value * periods + ns;
    while ( hpet_info->main_counter_value * periods < total_ns );
}
}     // namespace QuantumNEC::Kernel::x86_64
