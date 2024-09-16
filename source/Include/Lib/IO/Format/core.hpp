#pragma once
#include <Lib/IO/Format/format>
#include <Lib/STL/string>
#include <Lib/Types/Uefi.hpp>
PUBLIC namespace QuantumNEC::Lib::IO {
    auto print( const char *_fmt, auto &&..._args )->VOID {
        Lib::STL::string fmt { format( _fmt, _args... ) };
        for ( int i { }; i < fmt.size( ); ++i ) {
            // printf( "%c", fmt[ i ] );
        }
    }
    auto print( const char *_fmt )->VOID;
}
