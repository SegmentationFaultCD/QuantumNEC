#include <Lib/IO/Format/format>
PUBLIC namespace QuantumNEC::Lib::IO {
    template <>
    auto format( long double __val )->Lib::STL::string {
        Lib::STL::string buf_int { }, buf_float { };
        int integerPart { int( __val ) };
        long double floatPart { __val - int( __val ) };
        do {
            buf_int = char( integerPart % 10 + '0' ) + buf_int;
            integerPart /= 10;
        } while ( integerPart );
        do {
            floatPart *= 10;
        } while ( floatPart - int( floatPart ) >= 1e-6 );
        int floatPart__Val { int( floatPart ) };
        do {
            buf_float = char( floatPart__Val % 10 + '0' ) + buf_float;
            floatPart__Val /= 10;
        } while ( floatPart__Val );
        buf_int = buf_int + ".";
        buf_int = buf_int + buf_float;
        return STL::string { buf_int };
    }
}