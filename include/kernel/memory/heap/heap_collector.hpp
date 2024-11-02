#pragma once
#include <lib/Uefi.hpp>
PUBLIC namespace QuantumNEC::Kernel {
    PUBLIC class HeapCollector {
    public:
        explicit HeapCollector( VOID ) noexcept = default;
        virtual ~HeapCollector( VOID ) noexcept = default;

    public:
        auto free( IN VOID * ) -> VOID;
    };
}