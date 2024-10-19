#pragma once
#include <lib/Uefi.hpp>
#include <kernel/driver/acpi/xsdt.hpp>
PUBLIC namespace QuantumNEC::Kernel::x86_64 {
    PUBLIC class Fadt
    {
    public:
        struct FADT : Xsdt::ACPISDTHeader
        {
            explicit FADT( VOID ) noexcept = default;
            uint32_t firmwareControl;
            uint32_t dsdt;
            uint8_t reserved0;
            uint8_t preferredPMProfile;
            uint16_t sciInterrupt;
            uint32_t smiCommandPort;
            uint8_t acpiEnable;
            uint8_t acpiDisable;
            uint8_t s4BiosRequestValue;
            uint8_t pStateControlValue;
            uint32_t pm1aEventBlock;
            uint32_t pm1bEventBlock;
            uint32_t pm1aControlBlock;
            uint32_t pm1bControlBlock;
            uint32_t pm2ControlBlock;
            uint32_t powerManagementTimer;
            uint32_t gpEvent0Block;
            uint32_t gpEvent1Block;
            uint8_t pm1EventBlockLength;
            uint8_t pm1ControlBlockLength;
            uint8_t pm2ControlBlockLength;
            uint8_t pmTimerLength;
            uint8_t gpEvent0Length;
            uint8_t gpEvent1Length;
            uint8_t gpEvent1Offset;
            uint8_t cStateControlValue;
            uint16_t level2HardwareTimeout;
            uint16_t level3HardwareTimeout;
            uint16_t flushSize;
            uint16_t flushStride;
            uint8_t dutyOffset;
            uint8_t dutyWidth;
            uint8_t dayAlarm;
            uint8_t monthAlarm;
            uint8_t centuryRegister;     //	CMOS用来获取当前世纪
            uint16_t iaPcBootArch;
            uint8_t reserved1;
            uint32_t flags;
            Xsdt::GenericAddressStructure resetRegister;
            uint8_t resetValue;
            uint16_t armBootArch;
            uint8_t fadtMinorVersion;
            uint64_t xFirmwareControl;
            uint64_t xDsdt;
            Xsdt::GenericAddressStructure xPM1aEventBlock;
            Xsdt::GenericAddressStructure xPM1bEventBlock;
            Xsdt::GenericAddressStructure xPM1aControlBlock;
            Xsdt::GenericAddressStructure xPM1bControlBlock;
            Xsdt::GenericAddressStructure xPM2ControlBlock;
            Xsdt::GenericAddressStructure xPowerManagementTimerBlock;
            Xsdt::GenericAddressStructure xGPEvent0Block;
            Xsdt::GenericAddressStructure xGPEvent1Block;
            Xsdt::GenericAddressStructure sleepControlRegister;
            Xsdt::GenericAddressStructure sleepStatusRegister;
            uint64_t hypevisorID;
        } _packed;

        using table_type = FADT;

    public:
        explicit Fadt( IN Xsdt &xsdt ) noexcept;

    public:
        constexpr static auto signature { SIGN_32( 'F', 'A', 'C', 'P' ) };

    private:
        inline STATIC FADT *fadt;
    };
}