#include <Boot/graphics.hpp>
#include <Boot/file.hpp>
#include <Boot/boot.hpp>
#include <Protocol/GraphicsOutput.h>
#include <Protocol/LoadedImage.h>
#include <IndustryStandard/Bmp.h>
#include <Library/BmpSupportLib.h>
#include <Boot/output.hpp>
namespace {
inline STATIC EFI_GRAPHICS_OUTPUT_PROTOCOL *Gop { };
}
namespace QuantumNEC::Boot {
GraphicsService::GraphicsService( VOID ) noexcept {
    // 初始化图形界面
    uint64_t handle_count { };
    EFI_HANDLE *HandleBuffer { };

    if ( EFI_ERROR( gBS->LocateHandleBuffer( ByProtocol, &gEfiGraphicsOutputProtocolGuid, NULL,
                                             &handle_count, &HandleBuffer ) ) ) {
#ifdef DEBUG
        print( u"[ERROR] Failed to locate handle buffer for graphics.\n" );
#endif
        while ( true )
            ;
    }

    if ( EFI_ERROR( gBS->OpenProtocol( HandleBuffer[ 0 ], &gEfiGraphicsOutputProtocolGuid,     // 打开图形服务
                                       reinterpret_cast< VOID ** >( &Gop ), BootService::GlobalImageHandle,
                                       NULL, EFI_OPEN_PROTOCOL_GET_PROTOCOL ) ) ) {
#ifdef DEBUG
        print( u"[ERROR] Failed to open graphics protocol.\n" );
#endif
    }
    EFI_GRAPHICS_OUTPUT_MODE_INFORMATION *mode_info { };
    uint64_t mode_info_size { sizeof( EFI_GRAPHICS_OUTPUT_MODE_INFORMATION ) }, H { }, V { }, mode_index { };

    for ( uint64_t i { }; i < Gop->Mode->MaxMode; ++i ) {
        if ( EFI_ERROR( Gop->QueryMode( Gop, i, &mode_info_size, &mode_info ) ) ) {
            while ( true )
                ;
        }
        H = mode_info->HorizontalResolution;
        V = mode_info->VerticalResolution;

        if ( ( ( H == 1024 ) && ( V == 768 ) )     // 计算机所支持的分辨率
             || ( ( H == 1440 ) && ( V == 900 ) )
             || ( ( H == 1920 ) && ( V == 1080 ) ) ) {
            mode_index = i;
#ifdef DEBUG
            print( u"[DEBUG] max mode {}, hide {}, ver {}\n", Gop->Mode->MaxMode, H, V );
#endif
        }
    }
    if ( EFI_ERROR( Gop->SetMode( Gop, mode_index ) ) )
        while ( true )
            ;
    this->FrameBufferBase = Gop->Mode->FrameBufferBase;
    this->FrameBufferSize = Gop->Mode->FrameBufferSize;
    this->HorizontalResolution = Gop->Mode->Info->HorizontalResolution;
    this->VerticalResolution = Gop->Mode->Info->VerticalResolution;
    this->PixelsPerScanLine = Gop->Mode->Info->PixelsPerScanLine;

#ifdef DEBUG
    print( u"[SUCCESS] Initialize the graphics service management.\n" );
#endif
}
auto GraphicsService::display_logo( IN uchar_t *path ) noexcept -> VOID {
    auto logo_address { FileService { path }.read( ) };
    BmpConfig bmp { };
    auto &[ size, page_size, width, height, offset, pixel_start, tmp ] = bmp;
    auto bmp_translate = [ & ] {
        EFI_STATUS status { EFI_SUCCESS };
        auto conversion { [ & ]( uint64_t offset, uint64_t size ) -> uint64_t {
            uint64_t result { };
            for ( uint64_t i { }; i < size; ++i ) {
                result += *( reinterpret_cast< uint8_t * >( logo_address ) + offset + i ) << i * 8;
            }
            return result;
        } };

        size = conversion( 2, 4 );
        page_size = ( size >> 12 ) + 1;
        offset = conversion( 10, 4 );
        width = conversion( 18, 4 );
        height = conversion( 22, 4 );
        status = gBS->AllocatePages( AllocateAnyPages, EfiLoaderData,
                                     page_size, &pixel_start );
        auto pixel_from_file = reinterpret_cast< EFI_GRAPHICS_OUTPUT_BLT_PIXEL * >( logo_address + offset + width * height * 4 );
        auto pixel_to_buffer = reinterpret_cast< EFI_GRAPHICS_OUTPUT_BLT_PIXEL * >( pixel_start );
        for ( uint64_t i { }; i < height; i++ ) {
            pixel_from_file -= width;
            for ( uint64_t j { }; j < width; j++ ) {
                *pixel_to_buffer = *pixel_from_file;
                pixel_to_buffer++;
                pixel_from_file++;
            }
            pixel_from_file -= width;
        }
        return status;
    };
    if ( EFI_ERROR( bmp_translate( ) ) ) {
#ifdef DEBUG
        print( u"Wrong bmp image file format.\n" );
#endif
        while ( true )
            ;
    }

    Gop->Blt(
        Gop, reinterpret_cast< EFI_GRAPHICS_OUTPUT_BLT_PIXEL * >( pixel_start ),
        EfiBltBufferToVideo, 0, 0, ( Gop->Mode->Info->HorizontalResolution - width ) / 2, ( Gop->Mode->Info->VerticalResolution - height ) / 2, width, height,
        0 );
}

}     // namespace QuantumNEC::Boot