#pragma once
#include <lib/Uefi.hpp>
PUBLIC namespace QuantumNEC::Kernel {
    PUBLIC class KHeapCollector {
    public:
        explicit KHeapCollector( VOID ) noexcept = default;
        virtual ~KHeapCollector( VOID ) noexcept = default;

    public:
        auto free( IN VOID *virtual_address ) -> VOID;
    };
}