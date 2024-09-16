[Defines]
    PLATFORM_NAME = BootLoader
    PLATFORM_GUID = bb3426b8-6a0d-47f4-8568-fa94cd1fbd14
    PLATFORM_VERSION = 10001
    DSC_SPECIFICATION = 0x00010006
    SUPPORTED_ARCHITECTURES = X64 | IA32 | AARCH64 | ARM
    BUILD_TARGETS = DEBUG|RELEASE|NOOPT
    OUTPUT_DIRECTORY = Build/
[LibraryClasses]
    UefiLib|MdePkg/Library/UefiLib/UefiLib.inf
    UefiApplicationEntryPoint|MdePkg/Library/UefiApplicationEntryPoint/UefiApplicationEntryPoint.inf
    PrintLib|MdePkg/Library/BasePrintLib/BasePrintLib.inf
    PcdLib|MdePkg/Library/BasePcdLibNull/BasePcdLibNull.inf
    MemoryAllocationLib|MdePkg/Library/UefiMemoryAllocationLib/UefiMemoryAllocationLib.inf
    DebugLib|MdePkg/Library/UefiDebugLibConOut/UefiDebugLibConOut.inf
    BaseMemoryLib|MdePkg/Library/BaseMemoryLib/BaseMemoryLib.inf
    BaseLib|MdePkg/Library/BaseLib/BaseLib.inf
    UefiBootServicesTableLib|MdePkg/Library/UefiBootServicesTableLib/UefiBootServicesTableLib.inf
    DevicePathLib|MdePkg/Library/UefiDevicePathLib/UefiDevicePathLib.inf
    UefiRuntimeServicesTableLib|MdePkg/Library/UefiRuntimeServicesTableLib/UefiRuntimeServicesTableLib.inf
    DebugPrintErrorLevelLib|MdePkg/Library/BaseDebugPrintErrorLevelLib/BaseDebugPrintErrorLevelLib.inf
    BmpSupportLib|MdeModulePkg/Library/BaseBmpSupportLib/BaseBmpSupportLib.inf
    SafeIntLib|MdePkg/Library/BaseSafeIntLib/BaseSafeIntLib.inf
[Components]   
    QuantumNECPkg/Boot/Info/QuantumNECPkg.inf
[Buildoptions]
    GCC: *_*_*_CC_FLAGS = -w
    GCC: DEBUG_*_*_CC_FLAGS = -fno-rtti
    GCC: DEBUG_*_*_CC_FLAGS = -std=c++23
    GCC: DEBUG_*_*_CC_FLAGS = -fno-exceptions