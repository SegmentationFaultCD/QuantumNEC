#pragma once
#include <Lib/Types/Uefi.hpp>
#include <Lib/Types/type_bool.hpp>
PUBLIC namespace QuantumNEC::Lib::Base {
    PUBLIC class Allocate
    {
    public:
        typedef struct
        {
            Lib::Types::uint64_t index;           // 该条目的索引
            Lib::Types::uint64_t units_count;     // 该条目包含几个空置单元
        } AllocateTableEntry;

    private:
        typedef struct
        {
            Lib::Types::uint64_t entries_count;                // 条目总数
            Lib::Types::uint64_t frees_count;                  // 空闲条目数
            Lib::Types::Ptr< AllocateTableEntry > entries;     // 条目内容
        } AllocateTable;

    public:
        explicit( TRUE ) Allocate( VOID ) noexcept;
        explicit( TRUE ) Allocate( IN Lib::Types::uint64_t entries_count ) noexcept;
        explicit( TRUE ) Allocate( IN Lib::Types::R_Ref< Allocate > _table ) noexcept;

        virtual ~Allocate( VOID ) noexcept = default;

    public:
        auto operator=( IN Lib::Types::R_Ref< Allocate > _table ) noexcept -> Lib::Types::L_Ref< CONST Allocate >;

    public:
        /**
         * @brief 在表中分配单元
         * @param table 从这个表中分配
         * @param unitsCount 分配的单元个数
         * @return 分配到的单元下标
         */
        auto allocate( IN Types::uint64_t unitsCount ) -> Types::int64_t;
        /**
         * @brief 在表中释放单元
         * @param table 从这个表中释放
         * @param index 单元编号
         * @param unitsCount 释放的单元个数
         */
        auto free( IN Types::uint64_t index, IN Types::uint64_t unitsCount ) -> VOID;
        /**
         * @brief 统计空闲单元
         * @return 单元个数
         */
        auto get_free_total( VOID ) -> Lib::Types::uint64_t;

    private:
        Lib::Types::Ptr< AllocateTableEntry > entry_ { };
        AllocateTable table_ { };
    };
}