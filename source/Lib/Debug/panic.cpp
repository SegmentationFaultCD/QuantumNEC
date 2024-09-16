#include <Lib/Debug/panic.hpp>
PUBLIC namespace QuantumNEC::Lib::Debug {
    PUBLIC auto panic( IN Lib::Types::Ptr< CONST Lib::Types::char_t > message )->VOID {
        using Lib::IO::sout;
        using Lib::IO::ostream;
        using Lib::IO::endl;
        sout[ ostream::HeadLevel::DEBUG ] << "!!! ================== \tPanic Spin\t ================== !!!" << endl;
        sout[ ostream::HeadLevel::ERROR ] << "!!! ================== \t" << message << "\t ================== !!!" << endl;
        sout[ ostream::HeadLevel::DEBUG ] << "!!! ================== \tPanic Spin\t ================== !!!" << endl;
        while ( TRUE ) {
            Architecture::ArchitectureManager< TARGET_ARCH >::hlt( );
        }
    }
}