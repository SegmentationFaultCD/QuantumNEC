#include <Kernel/Memory/heap.hpp>
#include <Kernel/Memory/page.hpp>
#include <Lib/Base/symbol.hpp>
#include <Lib/STL/string>
PUBLIC namespace QuantumNEC::Lib::Base {
    Symbol::Symbol( IN Lib::Types::Ptr< SymbolData > _symbolTable, IN Lib::Types::int32_t _symbolNumber ) noexcept :
        symbolTable_ { _symbolTable },
        symbolNumber_ { _symbolNumber } {
    }
    Symbol::~Symbol( VOID ) noexcept {
    }
    auto Symbol::cleck_address_available( IN Lib::Types::uint64_t address )->Lib::Types::BOOL {
        return address >= Kernel::KERNEL_BASE_ADDRESS && address <= this->symbolTable_[ this->symbolNumber_ - 1 ].address;
    }

    auto Symbol::symbol_to_address( IN Lib::Types::Ptr< CONST Lib::Types::char_t > symbol )->Lib::Types::uint64_t {
        for ( auto i { 0 }; i < this->symbolNumber_; i++ ) {
            if ( Lib::STL::strcmp( symbol, this->symbolTable_[ i ].symbol ) == 0 ) {
                return this->symbolTable_[ i ].address;
            }
        }
        return 0;
    }

    auto Symbol::address_to_symbol( IN Lib::Types::uint64_t address )->Lib::Types::Ptr< Lib::Types::char_t > {
        if ( !this->cleck_address_available( address ) ) {
            return NULL;
        }
        for ( auto i { 0 }; i < this->symbolNumber_; i++ ) {
            if ( address >= this->symbolTable_[ i ].address && address < this->symbolTable_[ i + 1 ].address ) {
                return this->symbolTable_[ i ].symbol;
            }
        }
        return NULL;
    }
}