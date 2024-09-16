#pragma once
#include <Lib/Types/Uefi.hpp>
#include <Lib/Types/type_bool.hpp>
PUBLIC namespace QuantumNEC::Architecture {
    PUBLIC class Acpi
    {
    private:
        enum class ICSAttribute : Lib::Types::uint8_t {
            LOCAL_APIC = 0,
            I_O_APIC = 1,
            INTERRUPT_SOURCE_OVERRIDE = 2,
        };
        struct SDTHeader
        {
            Lib::Types::uint32_t signature;
            Lib::Types::uint32_t length;
            Lib::Types::uint8_t reserved;
            Lib::Types::uint8_t checksum;
            Lib::Types::uint8_t OEMID[ 6 ];
            Lib::Types::uint8_t OEMTableID[ 8 ];
            Lib::Types::uint32_t OEMReserved;
            Lib::Types::uint32_t creatorID;
            Lib::Types::uint32_t creatorReserved;
        } _packed;
        struct Xsdt : SDTHeader
        {
            Lib::Types::uintptr_t entries[ 0 ];
            auto operator[]( Lib::Types::size_t index ) CONST->Lib::Types::L_Ref< CONST SDTHeader > {
                Lib::Types::Ptr< CONST Lib::Types::uint64_t > entries { reinterpret_cast< Lib::Types::Ptr< CONST Lib::Types::uint64_t > >( this + 1 ) };
                return *reinterpret_cast< Lib::Types::Ptr< CONST SDTHeader > >( entries + index );
            }
            auto size( VOID ) CONST->Lib::Types::size_t {
                return ( this->length - sizeof( Xsdt ) ) / sizeof( Lib::Types::uint64_t );
            }
        } _packed;
        struct Rsdt : SDTHeader
        {
            Lib::Types::uint32_t entries[ 0 ];
            auto operator[]( Lib::Types::size_t index ) CONST->Lib::Types::L_Ref< CONST SDTHeader > {
                Lib::Types::Ptr< CONST Lib::Types::uint64_t > entries { reinterpret_cast< Lib::Types::Ptr< CONST Lib::Types::uint64_t > >( this + 1 ) };
                return *reinterpret_cast< Lib::Types::Ptr< CONST SDTHeader > >( entries + index );
            }
            auto size( VOID ) CONST->Lib::Types::size_t {
                return ( this->length - sizeof( Rsdt ) ) / sizeof( Lib::Types::uint64_t );
            }
        } _packed;
        struct Dsdt : SDTHeader
        {
            Lib::Types::uint8_t data[ 0 ];
        } _packed;
        struct Madt : SDTHeader
        {
            Lib::Types::uint32_t localApicAddress;
            Lib::Types::uint32_t flags;
        } _packed;
        struct MadtICS
        {
            ICSAttribute type;
            Lib::Types::uint8_t length;
        } _packed;

    public:
        struct LocalApic : MadtICS
        {
            Lib::Types::uint8_t acpiProcessorUID;
            Lib::Types::uint8_t apicID;
            Lib::Types::uint32_t flags;
        } _packed;
        struct IOApic : MadtICS
        {
            Lib::Types::uint8_t IOApicID;
            Lib::Types::uint8_t reserved;
            Lib::Types::uint32_t IOApicAddress;
            Lib::Types::uint32_t globalSystemInterruptBase;
        } _packed;
        struct InterruptSourceOverride : MadtICS
        {
            Lib::Types::uint8_t bus;
            Lib::Types::uint8_t source;
            Lib::Types::uint32_t globalSystemInterrupt;
            Lib::Types::uint16_t flags;
        } _packed;
        struct AcpiAddress
        {
            Lib::Types::uint8_t address_space_id;     // 0 - system memory, 1 - system I/O
            Lib::Types::uint8_t register_bit_width;
            Lib::Types::uint8_t register_bit_offset;
            Lib::Types::uint8_t access_width;
            Lib::Types::uint64_t address;
        } _packed;
        struct Hpet : SDTHeader
        {
            Lib::Types::uint8_t hardware_rev_id;
            Lib::Types::uint8_t comparator_count : 5;
            Lib::Types::uint8_t counter_size : 1;
            Lib::Types::uint8_t reserved : 1;
            Lib::Types::uint8_t legacy_replacement : 1;
            Lib::Types::uint16_t pci_vendor_id;
            AcpiAddress hpet_address;
            Lib::Types::uint8_t hpet_number;
            Lib::Types::uint16_t minimum_tick;
            Lib::Types::uint8_t page_protection;
        } _packed;
        struct Bgrt : SDTHeader
        {
            Lib::Types::uint16_t version;
            Lib::Types::uint8_t status;
            Lib::Types::uint8_t imageType;
            Lib::Types::Ptr< VOID > image;
            Lib::Types::uint32_t xOffset;
            Lib::Types::uint32_t yOffset;
        } _packed;
        struct Fadt : SDTHeader
        {
            Lib::Types::uint32_t firmwareControl;
            Lib::Types::uint32_t dsdt;
            Lib::Types::uint8_t reserved0;
            Lib::Types::uint8_t preferredPMProfile;
            Lib::Types::uint16_t sciInterrupt;
            Lib::Types::uint32_t smiCommandPort;
            Lib::Types::uint8_t acpiEnable;
            Lib::Types::uint8_t acpiDisable;
            Lib::Types::uint8_t s4BiosRequestValue;
            Lib::Types::uint8_t pStateControlValue;
            Lib::Types::uint32_t pm1aEventBlock;
            Lib::Types::uint32_t pm1bEventBlock;
            Lib::Types::uint32_t pm1aControlBlock;
            Lib::Types::uint32_t pm1bControlBlock;
            Lib::Types::uint32_t pm2ControlBlock;
            Lib::Types::uint32_t powerManagementTimer;
            Lib::Types::uint32_t gpEvent0Block;
            Lib::Types::uint32_t gpEvent1Block;
            Lib::Types::uint8_t pm1EventBlockLength;
            Lib::Types::uint8_t pm1ControlBlockLength;
            Lib::Types::uint8_t pm2ControlBlockLength;
            Lib::Types::uint8_t pmTimerLength;
            Lib::Types::uint8_t gpEvent0Length;
            Lib::Types::uint8_t gpEvent1Length;
            Lib::Types::uint8_t gpEvent1Offset;
            Lib::Types::uint8_t cStateControlValue;
            Lib::Types::uint16_t level2HardwareTimeout;
            Lib::Types::uint16_t level3HardwareTimeout;
            Lib::Types::uint16_t flushSize;
            Lib::Types::uint16_t flushStride;
            Lib::Types::uint8_t dutyOffset;
            Lib::Types::uint8_t dutyWidth;
            Lib::Types::uint8_t dayAlarm;
            Lib::Types::uint8_t monthAlarm;
            Lib::Types::uint8_t centuryRegister;     //	CMOS用来获取当前世纪
            Lib::Types::uint16_t iaPcBootArch;
            Lib::Types::uint8_t reserved1;
            Lib::Types::uint32_t flags;
            AcpiAddress resetRegister;
            Lib::Types::uint8_t resetValue;
            Lib::Types::uint16_t armBootArch;
            Lib::Types::uint8_t fadtMinorVersion;
            Lib::Types::uint64_t xFirmwareControl;
            Lib::Types::uint64_t xDsdt;
            AcpiAddress xPM1aEventBlock;
            AcpiAddress xPM1bEventBlock;
            AcpiAddress xPM1aControlBlock;
            AcpiAddress xPM1bControlBlock;
            AcpiAddress xPM2ControlBlock;
            AcpiAddress xPowerManagementTimerBlock;
            AcpiAddress xGPEvent0Block;
            AcpiAddress xGPEvent1Block;
            AcpiAddress sleepControlRegister;
            AcpiAddress sleepStatusRegister;
            Lib::Types::uint64_t hypevisorID;
        } _packed;
        class

        {
        private:
            Lib::Types::uint8_t globalSystemInterrupt_[ 24 ];

        public:
            auto operator[]( IN Lib::Types::uint8_t source ) -> Lib::Types::L_Ref< Lib::Types::uint8_t > {
                return globalSystemInterrupt_[ source ];
            }
            auto get( IN Lib::Types::uint8_t source ) -> Lib::Types::uint8_t {
                return globalSystemInterrupt_[ source ] != 0xff ? globalSystemInterrupt_[ source ] : source;
            }
        } inline STATIC globalSystemInterrupt;

    public:
        explicit Acpi( IN CONST Lib::Types::Ptr< Lib::Types::BootConfig > _config ) noexcept( true );

    public:
        virtual ~Acpi( VOID ) noexcept( true ) = default;

    public:
        template < typename T >
        auto is_valid( VOID ) CONST->Lib::Types::BOOL;

    public:
        inline STATIC Lib::Types::Ptr< VOID > io_apic_address { }, local_apic_address { };

    private:
        Lib::Types::Ptr< Lib::Types::Rsdp > rsdp { };
        Lib::Types::Ptr< Xsdt > xsdt { };

    public:
        inline STATIC Lib::Types::Ptr< Madt > madt { };
        inline STATIC Lib::Types::Ptr< Hpet > hpet { };
    };
    template <>
    auto Acpi::is_valid< Lib::Types::Rsdp >( VOID ) CONST->Lib::Types::BOOL;
}
