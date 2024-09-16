#pragma once
#define near
#define far
#define huge
#define Near
#define Far
#define Huge
#define NULL nullptr /* 空指针 */
namespace QuantumNEC::Lib::Types {
template < typename R, typename... Args >
using FuncPtr = R ( * )( Args... );
template < typename R, typename C, typename... Args >
using ClassFuncPtr = R ( C::* )( Args... );
template < typename T >
using Ptr = T *;
template < typename T >
using L_Ref = T &;
template < typename T >
using R_Ref = T &&;
using nullptr_t = decltype( nullptr );

}     // namespace QuantumNEC::Lib::Types