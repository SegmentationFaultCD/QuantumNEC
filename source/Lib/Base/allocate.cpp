#include <Lib/Base/allocate.hpp>
PUBLIC namespace QuantumNEC::Lib::Base {
    Allocate::Allocate( VOID ) noexcept {
    }
    Allocate::Allocate( IN Lib::Types::uint64_t entries_count ) noexcept {
        this->entry_ = new AllocateTableEntry[ entries_count ];
        this->table_.entries_count = entries_count;
        this->table_.frees_count = 0;
        this->table_.entries = this->entry_;
    }
    Allocate::Allocate( IN Lib::Types::R_Ref< Allocate > _table ) noexcept {
        this->table_.entries_count = _table.table_.entries_count;
        this->table_.frees_count = _table.table_.frees_count;
        this->table_.entries = _table.table_.entries;
    }
    auto Allocate::operator=( IN Lib::Types::R_Ref< Allocate > _table ) noexcept -> Lib::Types::L_Ref< CONST Allocate > {
        this->table_.entries_count = _table.table_.entries_count;
        this->table_.frees_count = _table.table_.frees_count;
        this->table_.entries = _table.table_.entries;
        return *this;
    }
    auto Allocate::allocate( IN Types::uint64_t units_count )->Types::int64_t {
        Lib::Types::uint64_t index { };
        for ( Lib::Types::uint64_t i { }; i < this->table_.entries_count; i++ ) {
            if ( this->table_.entries[ i ].units_count >= units_count ) {
                index = this->table_.entries[ i ].index;
                this->table_.entries[ i ].index += units_count;
                this->table_.entries[ i ].units_count -= units_count;
                if ( !( this->table_.entries[ i ].units_count ) ) {
                    this->table_.frees_count--;
                    while ( i < this->table_.frees_count ) {
                        this->table_.entries[ i ] = this->table_.entries[ i + 1 ];
                        i++;
                    }
                }
                return index;
            }
        }
        return -1;
    }
    auto Allocate::free( IN Types::uint64_t index, IN Types::uint64_t units_count )->VOID {
        Lib::Types::uint64_t i { }, j { };
        while ( i < this->table_.frees_count ) {
            if ( this->table_.entries[ i ].index > index ) {
                break;
            }
            ++i;
        }
        if ( i > 0 ) {
            if ( this->table_.entries[ i - 1 ].index + this->table_.entries[ i - 1 ].units_count == index ) {
                this->table_.entries[ i - 1 ].units_count += units_count;
                if ( i < this->table_.frees_count ) {
                    if ( index + units_count == this->table_.entries[ i ].index ) {
                        this->table_.entries[ i - 1 ].units_count += this->table_.entries[ i ].units_count;
                        this->table_.frees_count--;
                        while ( i < this->table_.frees_count ) {
                            this->table_.entries[ i ] = this->table_.entries[ i + 1 ];
                            i++;
                        }
                    }
                }
                return;
            }
        }
        if ( i < this->table_.frees_count ) {
            if ( index + units_count == this->table_.entries[ i ].index ) {
                this->table_.entries[ i ].index = index;
                this->table_.entries[ i ].units_count += units_count;
                return;
            }
        }
        if ( this->table_.frees_count < this->table_.entries_count ) {
            for ( j = this->table_.frees_count; j > i; --j ) {
                this->table_.entries[ j ] = this->table_.entries[ j - 1 ];
            }
            this->table_.frees_count++;
            this->table_.entries[ i ].index = index;
            this->table_.entries[ i ].units_count = units_count;
            return;
        }
        return;
    }
    auto Allocate::get_free_total( VOID )->Lib::Types::uint64_t {
        Lib::Types::uint64_t free_units { };
        for ( Lib::Types::uint64_t i { }; i < this->table_.frees_count; ++i ) {
            free_units += this->table_.entries[ i ].units_count;
        }
        return free_units;
    }
}