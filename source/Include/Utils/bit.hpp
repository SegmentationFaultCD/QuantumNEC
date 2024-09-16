#pragma once
#include <Lib/Types/Uefi.hpp>
#include <Lib/Types/type_base.hpp>
#include <Lib/Types/type_int.hpp>
#include <Lib/Types/type_ptr.hpp>
PUBLIC namespace QuantumNEC::Utils {
    PUBLIC auto bit_set( IN Lib::Types::Ptr< Lib::Types::uint64_t > addr, IN Lib::Types::uint64_t nr )->Lib::Types::uint64_t;

    PUBLIC auto bit_get( IN Lib::Types::Ptr< Lib::Types::uint64_t > addr, IN Lib::Types::uint64_t nr )->Lib::Types::uint64_t;

    PUBLIC auto bit_clean( IN Lib::Types::Ptr< Lib::Types::uint64_t > addr, IN Lib::Types::uint64_t nr )->Lib::Types::uint64_t;
}