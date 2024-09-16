#include <Arch/x86_64/platform/platform.hpp>
PUBLIC namespace QuantumNEC::Architecture {
    InterruptEntryRegister::InterruptEntryRegister( VOID ) noexcept :
        ClockEntry { }, SystemcallEntry { }, LocalApicEntry { }, RTCEntry { } {
    }
    InterruptEntryRegister::~InterruptEntryRegister( VOID ) noexcept {
    }
}