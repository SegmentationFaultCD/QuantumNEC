#pragma once
#include <Lib/Types/Uefi.hpp>
#ifdef DEBUG
#define ASSERT ( CONDITION )
#else
#define ASSERT( CONDITION ) \
    ( ( CONDITION ) || QuantumNEC::Lib::Debug::assert( __FILE__, __LINE__, __func__, #CONDITION ) );
#endif
PUBLIC namespace QuantumNEC::Lib::Debug {
    PUBLIC auto assert(
        IN CONST Lib::Types::Ptr< Lib::Types::char_t > filename,
        IN CONST Lib::Types::int32_t line,
        IN CONST Lib::Types::Ptr< Lib::Types::char_t > func,
        IN CONST Lib::Types::Ptr< Lib::Types::char_t > cmd )
        ->VOID;
}
