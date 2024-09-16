#pragma once
#include <Lib/Types/Uefi.hpp>

PUBLIC namespace QuantumNEC::Architecture {
    PUBLIC class Beep     // 蜂鸣器管理
    {
    public:
        explicit Beep( VOID ) noexcept;
        virtual ~Beep( VOID ) noexcept;

    private:
        STATIC auto enable_beep( VOID ) -> VOID;
        STATIC auto disable_beep( VOID ) -> VOID;

    public:
        STATIC auto beep( VOID ) -> VOID {
            enable_beep( );
            disable_beep( );
        }

    private:
        inline STATIC Lib::Types::BOOL beeping { };
    };
}