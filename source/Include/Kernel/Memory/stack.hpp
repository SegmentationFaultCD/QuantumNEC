#pragma once
#include <Lib/Types/Uefi.hpp>
PUBLIC namespace QuantumNEC::Kernel {
    PUBLIC class StackMemory
    {
    public:
        explicit StackMemory( VOID ) noexcept = default;
        virtual ~StackMemory( VOID ) noexcept = default;

    public:
        STATIC auto malloc( IN Lib::Types::size_t ) -> Lib::Types::Ptr< VOID >;
        STATIC auto free( IN Lib::Types::Ptr< VOID > ) -> VOID;
    };
}     // namespace QuantumNEC::Kernel::Memory
