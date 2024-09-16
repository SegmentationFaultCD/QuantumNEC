#pragma once
#include <Boot/base.hpp>
#include <Boot/memory.hpp>
namespace QuantumNEC::Boot {
class BootService
{
public:
    explicit BootService( IN EFI_HANDLE ImageHandle, IN EFI_SYSTEM_TABLE *SystemTable ) noexcept;
    ~BootService( VOID ) noexcept {
    }

public:
    /**
     * @brief 禁用计时器
     */
    auto close_timer( VOID ) -> EFI_STATUS {
        // 禁用看门狗计时器
        return gBS->SetWatchdogTimer( 0, 0, 0, NULL );
    }
    /**
     * @brief 退出启动时服务
     */
    auto exit_boot_service( IN MemoryService &ms ) {
        return gBS->ExitBootServices( GlobalImageHandle, ms.get_memory_key( ) );
    }

    inline STATIC EFI_HANDLE GlobalImageHandle { };

    inline STATIC EFI_SYSTEM_TABLE *GlobalSystemTable { };
};

}     // namespace QuantumNEC::Boot
