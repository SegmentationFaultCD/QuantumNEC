#include <Boot/boot.hpp>
#include <Boot/graphics.hpp>
#include <Boot/acpi.hpp>
#include <Boot/font.hpp>
#include <Boot/kloader.hpp>
#include <Boot/output.hpp>
using namespace QuantumNEC::Boot;
extern "C" auto EFIAPI UefiMain( IN EFI_HANDLE ImageHandle, IN EFI_SYSTEM_TABLE *SystemTable ) -> EFI_STATUS {
#ifdef DEBUG
    print( u"[INFO] Has enterd UEFI Boot Loader.\n" );
#endif
    BootService boot { ImageHandle, SystemTable };
#ifdef DEBUG
    print( u"[INFO] Initialization graphics driver.\n" );
#endif
    GraphicsService graphics_data { };
    graphics_data.display_logo( u"\\EFI\\Boot\\Logo.BMP" );
    MemoryService memory_map { };
    AcpiService acpi_table { };
    FontLoaderService font_frame { u"\\QuantumNEC\\SYSTEM64\\Unicode.dll" };
    KernelLoaderService kernel_loader { u"\\QuantumNEC\\microKernel.elf" };
    boot.close_timer( );
    boot.exit_boot_service( memory_map );

    return kernel_loader.call_kernel_entry( graphics_data, memory_map, acpi_table, font_frame );
}