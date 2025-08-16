#pragma once
// #include <atomic>
#include <concepts>
#include <cstdint>
#include <limine.h>
namespace Memory {
inline std::uint64_t __kvddr_offset__ = 0ul;

inline auto hhdm_initialize( limine_hhdm_response *res ) {
    __kvddr_offset__ = res->offset;
}

template < typename T >
    requires std::convertible_to< T, std::uint64_t > || requires( T p ) { reinterpret_cast< std::uint64_t >( p ); }
inline auto physical_to_virtual( T paddress ) {
    return (std::uint64_t)paddress + __kvddr_offset__;
}
template <>
inline auto physical_to_virtual< void * >( void *paddress ) {
    return (std::uint64_t)paddress + __kvddr_offset__;
}
template < typename T >
    requires std::convertible_to< T, std::uint64_t > || requires( T p ) { reinterpret_cast< std::uint64_t >( p ); }
inline auto virtual_to_physical( T vaddress ) {
    return (std::uint64_t)vaddress - __kvddr_offset__;
}
template <>
inline auto virtual_to_physical< void * >( void *vaddress ) {
    return (std::uint64_t)vaddress - __kvddr_offset__;
}
}     // namespace Memory
