#pragma once
#include <Lib/Uefi.hpp>
#include <Arch/Arch.hpp>
PUBLIC namespace QuantumNEC::Lib {
    PUBLIC auto panic( IN CONST char_t * message )->VOID;
    PUBLIC template < typename... Msgs >
    auto panic( IN Msgs... messages )->VOID {
        // sout[ ostream::HeadLevel::DEBUG ] << "!!! ================== \tPanic Spin\t ================== !!!" << endl;
        // sout[ ostream::HeadLevel::ERROR ];
        // ( ( sout << '\t' << messages ), ... );
        // endl( sout );
        // sout[ ostream::HeadLevel::DEBUG ] << "!!! ================== \tPanic Spin\t ================== !!!" << endl;
        while ( TRUE )
            Architecture::ArchitectureManager< TARGET_ARCH >::hlt( );
    }
}
#define Panic( ... ) QuantumNEC::Lib::panic( __FILE__, __LINE__, __func__, #__VA_ARGS__ )