#include <Lib/IO/Stream/iostream>
PUBLIC namespace QuantumNEC::Lib::IO {
    basic_ostream< Types::char_t >::basic_ostream( VOID ) :
        BasePrint { } {
    }
    basic_ostream< Types::char_t >::basic_ostream( IN Types::R_Ref< basic_ostream< Types::char_t > > ) noexcept {
    }
    auto basic_ostream< Types::char_t >::operator=( IN Types::R_Ref< basic_ostream< Types::char_t > > osm )->Types::L_Ref< basic_ostream< Types::char_t > > {
        this->display = osm.display;
        return *this;
    }
    auto basic_ostream< Types::char_t >::put( IN Types::char_t __ch )->Types::L_Ref< basic_ostream< Types::char_t > > {
        this->printk( this->display.FColor, this->display.BColor, "%c", __ch );
        return *this;
    }
    auto basic_ostream< Types::char_t >::write( IN Types::Ptr< Types::char_t > __str, IN Types::size_t __count )->Types::L_Ref< basic_ostream< Types::char_t > > {
        while ( *__str && __count-- ) {
            this->printk( this->display.FColor, this->display.BColor, "%c",
                          *__str );
        }
        return *this;
    }
    auto basic_ostream< Types::char_t >::operator[]( IN Lib::Types::R_Ref< HeadLevel > level )->Lib::Types::L_Ref< basic_ostream< Lib::Types::char_t > > {
        switch ( level ) {
        case HeadLevel::INFO: {
            this->display.FColor = DisplayColor::INDIGO;
            this->printk( DisplayColor::WHITE, this->display.BColor, "[ " );
            this->printk( this->display.FColor, this->display.BColor, "INFO" );
            this->printk( DisplayColor::WHITE, this->display.BColor, " ]     " );
            this->display.FColor = DisplayColor::WHITE;
            break;
        }
        case HeadLevel::DEBUG: {
            this->display.FColor = DisplayColor::GRAYISH;
            this->printk( DisplayColor::WHITE, this->display.BColor, "[ " );
            this->printk( this->display.FColor, this->display.BColor, "DEBUG" );
            this->printk( DisplayColor::WHITE, this->display.BColor, " ]    " );
            this->display.FColor = DisplayColor::WHITE;
            break;
        }
        case HeadLevel::ERROR: {
            this->display.FColor = DisplayColor::RED;
            this->printk( DisplayColor::WHITE, this->display.BColor, "[ " );
            this->printk( this->display.FColor, this->display.BColor, "ERROR" );
            this->printk( DisplayColor::WHITE, this->display.BColor, " ]    " );
            this->display.FColor = DisplayColor::WHITE;
            break;
        }
        case HeadLevel::SYSTEM: {
            this->display.FColor = DisplayColor::BLUEGRAY;
            this->printk( DisplayColor::WHITE, this->display.BColor, "[ " );
            this->printk( this->display.FColor, this->display.BColor, "SYSTEM" );
            this->printk( DisplayColor::WHITE, this->display.BColor, " ]   " );
            this->display.FColor = DisplayColor::WHITE;
            break;
        }
        case HeadLevel::USER: {
            this->display.FColor = DisplayColor::GRAY;
            this->printk( DisplayColor::WHITE, this->display.BColor, "[ " );
            this->printk( this->display.FColor, this->display.BColor, "USER" );
            this->printk( DisplayColor::WHITE, this->display.BColor, " ]     " );
            this->display.FColor = DisplayColor::WHITE;
            break;
        }
        case HeadLevel::OK: {
            this->display.FColor = DisplayColor::GREEN;
            this->printk( DisplayColor::WHITE, this->display.BColor, "[ " );
            this->printk( this->display.FColor, this->display.BColor, "OK/READY" );
            this->printk( DisplayColor::WHITE, this->display.BColor, " ] " );
            this->display.FColor = DisplayColor::WHITE;
            break;
        }
        case HeadLevel::START: {
            this->display.FColor = DisplayColor::YELLOW;
            this->printk( DisplayColor::WHITE, this->display.BColor, "[ " );
            this->printk( this->display.FColor, this->display.BColor, "START" );
            this->printk( DisplayColor::WHITE, this->display.BColor, " ]    " );
            this->display.FColor = DisplayColor::WHITE;
            break;
        }
        case HeadLevel::WARNING: {
            this->display.FColor = DisplayColor::ORANGE;
            this->printk( DisplayColor::WHITE, this->display.BColor, "[ " );
            this->printk( this->display.FColor, this->display.BColor, "WARNING" );
            this->printk( DisplayColor::WHITE, this->display.BColor, " ] " );
            this->display.FColor = DisplayColor::WHITE;
            break;
        }
        };
        return *this;
    }
    template <>
    auto basic_ostream< Types::char_t >::operator<< < Types::int16_t >(
        IN Types::int16_t __val )
        ->Types::L_Ref< basic_ostream< Types::char_t > > {
        this->printk( this->display.FColor, this->display.BColor, "%d", __val );
        return *this;
    }
    template <>
    auto basic_ostream< Types::char_t >::operator<< < Types::int32_t >(
        IN Types::int32_t __val )
        ->Types::L_Ref< basic_ostream< Types::char_t > > {
        this->printk( this->display.FColor, this->display.BColor, "%d", __val );
        return *this;
    }
    template <>
    auto basic_ostream< Types::char_t >::operator<< < Types::int64_t >(
        IN Types::int64_t __val )
        ->Types::L_Ref< basic_ostream< Types::char_t > > {
        this->operator<<( (Types::int128_t)__val );
        return *this;
    }
    template <>
    auto basic_ostream< Types::char_t >::operator<< < Types::int128_t >(
        IN Types::int128_t __val )
        ->Types::L_Ref< basic_ostream< Types::char_t > > {
        this->printk( this->display.FColor, this->display.BColor, "%ld", __val );
        return *this;
    }
    template <>
    auto basic_ostream< Types::char_t >::operator<< < Types::uint16_t >(
        IN Types::uint16_t __val )
        ->Types::L_Ref< basic_ostream< Types::char_t > > {
        this->printk( this->display.FColor, this->display.BColor, "%u", __val );
        return *this;
    }
    template <>
    auto basic_ostream< Types::char_t >::operator<< < Types::uint32_t >(
        IN Types::uint32_t __val )
        ->Types::L_Ref< basic_ostream< Types::char_t > > {
        this->printk( this->display.FColor, this->display.BColor, "%u", __val );
        return *this;
    }
    template <>
    auto basic_ostream< Types::char_t >::operator<< < Types::uint64_t >(
        IN Types::uint64_t __val )
        ->Types::L_Ref< basic_ostream< Types::char_t > > {
        this->operator<<( (Types::uint128_t)__val );
        return *this;
    }
    template <>
    auto basic_ostream< Types::char_t >::operator<< < Types::uint128_t >(
        IN Types::uint128_t __val )
        ->Types::L_Ref< basic_ostream< Types::char_t > > {
        this->printk( this->display.FColor, this->display.BColor, "%lu", __val );
        return *this;
    }
    template <>
    auto basic_ostream< Types::char_t >::operator<< < Types::uint8_t >(
        IN Types::uint8_t __val )
        ->Types::L_Ref< basic_ostream< Types::char_t > > {
        this->printk( this->display.FColor, this->display.BColor, "%d", __val );
        return *this;
    }
    template <>
    auto basic_ostream< Types::char_t >::operator<< < Types::char_t >(
        IN Types::char_t __val )
        ->Types::L_Ref< basic_ostream< Types::char_t > > {
        this->printk( this->display.FColor, this->display.BColor, "%c", __val );
        return *this;
    }
    template <>
    auto basic_ostream< Types::char_t >::operator<< < Types::Ptr< Types::char_t > >(
        IN Types::Ptr< Types::char_t > __val )
        ->Types::L_Ref< basic_ostream< Types::char_t > > {
        this->operator<<( reinterpret_cast< Types::Ptr< CONST Types::char_t > >( __val ) );
        return *this;
    }
    template <>
    auto basic_ostream< Types::char_t >::operator<< < Types::Ptr< CONST Types::char_t > >(
        IN Types::Ptr< CONST Types::char_t > __val )
        ->Types::L_Ref< basic_ostream< Types::char_t > > {
        this->printk( this->display.FColor, this->display.BColor, "%s", __val );
        return *this;
    }
    template <>
    auto basic_ostream< Types::char_t >::operator<< < Types::double32_t >(
        IN Types::double32_t __val )
        ->Types::L_Ref< basic_ostream< Types::char_t > > {
        this->printk( this->display.FColor, this->display.BColor, "%f", __val );
        return *this;
    }
    template <>
    auto basic_ostream< Types::char_t >::operator<< < Types::double64_t >(
        IN Types::double64_t __val )
        ->Types::L_Ref< basic_ostream< Types::char_t > > {
        this->printk( this->display.FColor, this->display.BColor, "%lf", __val );
        return *this;
    }
    template <>
    auto basic_ostream< Types::char_t >::operator<< < Types::double128_t >(
        IN Types::double128_t __val )
        ->Types::L_Ref< basic_ostream< Types::char_t > > {
        this->printk( this->display.FColor, this->display.BColor, "%llf", __val );
        return *this;
    }
    template <>
    auto basic_ostream< Types::char_t >::operator<< < long >( IN long __val )
        ->Types::L_Ref< basic_ostream< Types::char_t > > {
        this->printk( this->display.FColor, this->display.BColor, "%ld", __val );
        return *this;
    }
    template <>
    auto basic_ostream< Types::char_t >::operator<< < unsigned long >(
        IN unsigned long __val )
        ->Types::L_Ref< basic_ostream< Types::char_t > > {
        this->printk( this->display.FColor, this->display.BColor, "%lu", __val );
        return *this;
    }
    auto basic_ostream< Types::char_t >::operator( )(
        IN Lib::Types::R_Ref< DisplayColor > __F,
        IN Lib::Types::R_Ref< DisplayColor > __B )
        ->Lib::Types::L_Ref< basic_ostream< Lib::Types::char_t > > {
        this->display.FColor = __F;
        this->display.BColor = __B;
        return *this;
    }
    template <>
    auto basic_ostream< Types::char_t >::operator<< < Types::FuncPtr< VOID, Types::L_Ref< basic_ostream< Lib::Types::char_t > > > >(
        IN Types::FuncPtr< VOID, Types::L_Ref< basic_ostream< Lib::Types::char_t > > > )
        ->Types::L_Ref< basic_ostream< Types::char_t > > {
        this->printk( this->display.FColor, this->display.BColor, "\n\r" );
        return *this;
    }
    template <>
    auto basic_ostream< Types::char_t >::operator<< < Lib::Types::Ptr< Lib::Types::uint16_t > >( IN Lib::Types::Ptr< Lib::Types::uint16_t > __val )->Types::L_Ref< basic_ostream< Types::char_t > > {
        this->printk( this->display.FColor, this->display.BColor, "%p", __val );
        return *this;
    }
    template <>
    auto basic_ostream< Types::char_t >::operator<< < Lib::Types::Ptr< Lib::Types::uint32_t > >( IN Lib::Types::Ptr< Lib::Types::uint32_t > __val )->Types::L_Ref< basic_ostream< Types::char_t > > {
        this->printk( this->display.FColor, this->display.BColor, "%p", __val );
        return *this;
    }
    template <>
    auto basic_ostream< Types::char_t >::operator<< < Lib::Types::Ptr< unsigned long > >( IN Lib::Types::Ptr< unsigned long > __val )->Types::L_Ref< basic_ostream< Types::char_t > > {
        this->printk( this->display.FColor, this->display.BColor, "%p", __val );
        return *this;
    }
    template <>
    auto basic_ostream< Types::char_t >::operator<< < Lib::Types::Ptr< Lib::Types::uint64_t > >( IN Lib::Types::Ptr< Lib::Types::uint64_t > __val )->Types::L_Ref< basic_ostream< Types::char_t > > {
        this->printk( this->display.FColor, this->display.BColor, "%p", __val );
        return *this;
    }
    template <>
    auto basic_ostream< Types::char_t >::operator<< < Lib::Types::Ptr< Lib::Types::uint128_t > >( IN Lib::Types::Ptr< Lib::Types::uint128_t > __val )->Types::L_Ref< basic_ostream< Types::char_t > > {
        this->printk( this->display.FColor, this->display.BColor, "%p", __val );
        return *this;
    }
    template <>
    auto basic_ostream< Types::char_t >::operator<< < Lib::Types::Ptr< Lib::Types::int16_t > >( IN Lib::Types::Ptr< Lib::Types::int16_t > __val )->Types::L_Ref< basic_ostream< Types::char_t > > {
        this->printk( this->display.FColor, this->display.BColor, "%p", __val );
        return *this;
    }
    template <>
    auto basic_ostream< Types::char_t >::operator<< < Lib::Types::Ptr< Lib::Types::int32_t > >( IN Lib::Types::Ptr< Lib::Types::int32_t > __val )->Types::L_Ref< basic_ostream< Types::char_t > > {
        this->printk( this->display.FColor, this->display.BColor, "%p", __val );
        return *this;
    }
    template <>
    auto basic_ostream< Types::char_t >::operator<< < Lib::Types::Ptr< long > >( IN Lib::Types::Ptr< long > __val )->Types::L_Ref< basic_ostream< Types::char_t > > {
        this->printk( this->display.FColor, this->display.BColor, "%p", __val );
        return *this;
    }
    template <>
    auto basic_ostream< Types::char_t >::operator<< < Lib::Types::Ptr< Lib::Types::int64_t > >( IN Lib::Types::Ptr< Lib::Types::int64_t > __val )->Types::L_Ref< basic_ostream< Types::char_t > > {
        this->printk( this->display.FColor, this->display.BColor, "%p", __val );
        return *this;
    }
    template <>
    auto basic_ostream< Types::char_t >::operator<< < Lib::Types::Ptr< Lib::Types::int128_t > >( IN Lib::Types::Ptr< Lib::Types::int128_t > __val )->Types::L_Ref< basic_ostream< Types::char_t > > {
        this->printk( this->display.FColor, this->display.BColor, "%p", __val );
        return *this;
    }
    template <>
    auto basic_ostream< Types::char_t >::operator<< < Lib::Types::Ptr< VOID > >( IN Lib::Types::Ptr< VOID > __val )->Types::L_Ref< basic_ostream< Types::char_t > > {
        this->printk( this->display.FColor, this->display.BColor, "%p", __val );
        return *this;
    }
    template <>
    auto basic_ostream< Types::char_t >::operator<< < Lib::Types::BOOL >( IN Lib::Types::BOOL __val )->Types::L_Ref< basic_ostream< Types::char_t > > {
        this->printk( this->display.FColor, this->display.BColor, "%d", static_cast< Lib::Types::int32_t >( __val ) );
        return *this;
    }
    template <>
    auto basic_ostream< Types::char_t >::operator<< < Lib::Types::R_Ref< ios::ios_base > >( IN Lib::Types::R_Ref< ios::ios_base > )->Types::L_Ref< basic_ostream< Types::char_t > > {
        // 写入string
        return *this;
    }
}