#pragma once
#include <Lib/Types/Uefi.hpp>
#include <Lib/Types/type_bool.hpp>
PUBLIC namespace QuantumNEC::Lib::Base {
    PUBLIC class Symbol
    {
    private:
        typedef struct
        {
            Lib::Types::uint64_t address;
            Lib::Types::Ptr< Lib::Types::char_t > symbol;
        } SymbolData;

    public:
        explicit( true ) Symbol( IN Lib::Types::Ptr< SymbolData > _symbolTable, IN Lib::Types::int32_t _symbolNumber ) noexcept;

    public:
        virtual ~Symbol( VOID ) noexcept;

    public:
        /**
         * @brief 检查地址是否可用
         * @param address 要检查的地址
         */
        auto cleck_address_available( IN Lib::Types::uint64_t address ) -> Lib::Types::BOOL;
        /**
         * @brief 标签到地址的转换
         * @param symbol 标签
         * @return 转换后的地址
         */
        auto symbol_to_address( IN Lib::Types::Ptr< CONST Lib::Types::char_t > symbol ) -> Lib::Types::uint64_t;
        /**
         * @brief 地址到标签的转换
         * @param address 地址
         * @return 转换后的标签
         */
        auto address_to_symbol( IN Lib::Types::uint64_t address ) -> Lib::Types::Ptr< Lib::Types::char_t >;

    private:
        Lib::Types::Ptr< SymbolData > symbolTable_ { };
        Lib::Types::int32_t symbolNumber_ { };
    };
}