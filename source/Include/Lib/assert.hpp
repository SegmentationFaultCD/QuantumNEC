#pragma once
#include <Lib/Uefi.hpp>
#ifdef DEBUG
#define ASSERT ( CONDITION )
#else
#define ASSERT( CONDITION ) \
    ( ( CONDITION ) || QuantumNEC::Lib::assert( __FILE__, __LINE__, __func__, #CONDITION ) );
#endif
PUBLIC namespace QuantumNEC::Lib {
    PUBLIC auto assert(
        IN CONST char_t * filename,
        IN CONST int32_t line,
        IN CONST char_t * func,
        IN CONST char_t * cmd )
        ->VOID;
}
