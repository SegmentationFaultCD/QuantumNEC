#include <Lib/assert.hpp>

PUBLIC namespace QuantumNEC::Lib {
    PUBLIC auto assert(
        [[maybe_unused]] IN CONST char_t * filename,
        [[maybe_unused]] IN CONST int32_t line,
        [[maybe_unused]] IN CONST char_t * func,
        [[maybe_unused]] IN CONST char_t * cmd )
        -> VOID {
        // sout[ ostream::HeadLevel::DEBUG ] << "!!! Assert !!!" << endl;
        // sout[ ostream::HeadLevel::DEBUG ] << "File Name -> " << filename << endl;
        // sout[ ostream::HeadLevel::DEBUG ] << "Assert Line -> " << line << endl;
        // sout[ ostream::HeadLevel::DEBUG ] << "Assert Function -> " << func << endl;
        // sout[ ostream::HeadLevel::DEBUG ] << cmd << endl;
        // sout[ ostream::HeadLevel::DEBUG ] << "!!! Assert !!!" << endl;
    }
}