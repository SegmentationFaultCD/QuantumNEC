#include <Lib/Debug/assert.hpp>
#include <Lib/IO/Stream/iostream>
#include <Arch/Arch.hpp>
PUBLIC namespace QuantumNEC::Lib::Debug {
    PUBLIC auto assert(
        IN CONST Lib::Types::Ptr< Lib::Types::char_t > filename,
        IN CONST Lib::Types::int32_t line,
        IN CONST Lib::Types::Ptr< Lib::Types::char_t > func,
        IN CONST Lib::Types::Ptr< Lib::Types::char_t > cmd )
        ->VOID {
        using namespace QuantumNEC::Lib::IO;
        sout[ ostream::HeadLevel::DEBUG ] << "!!! Assert !!!" << endl;
        sout[ ostream::HeadLevel::DEBUG ] << "File Name -> " << filename << endl;
        sout[ ostream::HeadLevel::DEBUG ] << "Assert Line -> " << line << endl;
        sout[ ostream::HeadLevel::DEBUG ] << "Assert Function -> " << func << endl;
        sout[ ostream::HeadLevel::DEBUG ] << cmd << endl;
        sout[ ostream::HeadLevel::DEBUG ] << "!!! Assert !!!" << endl;
        while ( TRUE ) {
            Architecture::ArchitectureManager< TARGET_ARCH >::hlt( );
        }
    }
}