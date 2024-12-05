#pragma once
#include <lib/Uefi.hpp>
PUBLIC namespace QuantumNEC::Kernel::x86_64 {
    PUBLIC class PCSpeaker     // 蜂鸣器管理
    {
    public:
        explicit PCSpeaker( VOID ) noexcept;
        virtual ~PCSpeaker( VOID ) noexcept;

    private:
        static auto enable( VOID ) -> VOID;
        static auto disable( VOID ) -> VOID;

    public:
        static auto beep( VOID ) -> VOID;
    };
}