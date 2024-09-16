#pragma once
#include <Lib/Uefi.hpp>

PUBLIC namespace QuantumNEC::Architecture {
    PUBLIC class PCSpeaker     // 蜂鸣器管理
    {
    public:
        explicit PCSpeaker( VOID ) noexcept;
        virtual ~PCSpeaker( VOID ) noexcept;

    private:
        STATIC auto enable_beep( VOID ) -> VOID;
        STATIC auto disable_beep( VOID ) -> VOID;

    public:
        STATIC auto beep( VOID ) -> VOID {
            enable_beep( );
            disable_beep( );
        }

    private:
        inline STATIC BOOL beeping { };
    };
}