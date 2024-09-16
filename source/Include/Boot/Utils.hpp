#pragma once
#include <Boot/base.hpp>
VOID *operator new( unsigned long int size ) noexcept;
VOID *operator new[]( unsigned long int cb ) noexcept;
VOID *operator new( unsigned long int, VOID *address ) noexcept;
VOID operator delete( VOID *, unsigned long int ) noexcept;
VOID operator delete( VOID *address ) noexcept;
VOID operator delete[]( VOID *address ) noexcept;