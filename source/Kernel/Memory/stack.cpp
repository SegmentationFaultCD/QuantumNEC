#include <Kernel/Memory/stack.hpp>
#include <Lib/STL/string>
PUBLIC namespace {
    using namespace QuantumNEC;
    Lib::Types::char_t stackMemoryBuffer[ 65535 ] { };
    Lib::Types::Ptr< Lib::Types::char_t > buffer { stackMemoryBuffer };
}
PUBLIC namespace QuantumNEC::Kernel {
    auto StackMemory::malloc( IN Lib::Types::size_t _size )->Lib::Types::Ptr< VOID > {
        if ( !_size ) { return NULL; }
        buffer += _size;
        Lib::STL::memset( buffer, 0, _size );
        return buffer - _size;
    }
    auto StackMemory::free( IN Lib::Types::Ptr< VOID > address )->VOID {
        if ( address ) {
            Lib::Types::size_t size { static_cast< Lib::Types::size_t >( reinterpret_cast< Lib::Types::Ptr< Lib::Types::char_t > >( address ) - buffer ) };
            Lib::STL::memset( address, 0, size );
            buffer -= size;
        }
    }
}