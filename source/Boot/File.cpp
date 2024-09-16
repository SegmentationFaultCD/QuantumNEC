#include <Boot/file.hpp>
#include <Boot/boot.hpp>
#include <Guid/FileInfo.h>
#include <Protocol/SimpleFileSystem.h>
#include <Boot/output.hpp>
namespace QuantumNEC::Boot {
FileService::FileService( IN uchar_t *path ) noexcept {
    uint64_t handle_count { };
    EFI_HANDLE *handle_buffer { };
    // 打开文件服务

    if ( EFI_ERROR( gBS->LocateHandleBuffer( ByProtocol, &gEfiSimpleFileSystemProtocolGuid, NULL, &handle_count, &handle_buffer ) ) ) {
#ifdef DEBUG
        print( u"[ERROR] Failed to LocateHanleBuffer of SimpleFileSystemProtocol.\n" );
#endif
        while ( TRUE )
            ;
    }

    EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *FileSystem { };
    if ( EFI_ERROR( gBS->OpenProtocol(
             handle_buffer[ 0 ], &gEfiSimpleFileSystemProtocolGuid,
             reinterpret_cast< VOID ** >( &FileSystem ), BootService::GlobalImageHandle, NULL,
             EFI_OPEN_PROTOCOL_GET_PROTOCOL ) ) ) {
#ifdef DEBUG
        print( u"[ERROR] Failed to open first handle that supported SimpleFileSystemProtocol.\n" );
#endif
        while ( TRUE )
            ;
    }

    // 获取文件头
    if ( EFI_ERROR( FileSystem->OpenVolume( FileSystem, &this->root ) ) ) {
#ifdef DEBUG
        print( u"[ERROR] Failed to open volume.\n" );
#endif
        while ( TRUE )
            ;
    }

    // 读取这个文件到内存
    if ( EFI_ERROR( this->root->Open( this->root, &this->file_protocol, reinterpret_cast< CHAR16 * >( path ),
                                      EFI_FILE_MODE_READ | EFI_FILE_MODE_WRITE,
                                      EFI_OPEN_PROTOCOL_GET_PROTOCOL ) ) ) {
#ifdef DEBUG
        print( u"[ERROR] Failed to open file.\n" );
#endif
        while ( TRUE )
            ;
        if ( EFI_ERROR( this->root->Close( this->root ) ) ) {
#ifdef DEBUG
            print( u"[ERROR] Failed to close file.\n" );
#endif
            while ( TRUE )
                ;
        }
    }
}
auto FileService::read( VOID ) -> uint64_t {
    uint64_t file_base { };
    auto info_size { sizeof( EFI_FILE_INFO ) + 128ull };
    auto file_info { new EFI_FILE_INFO[ info_size ] };
    // 为文件信息描述表申请内存
    // 从文件获取内容
    if ( EFI_ERROR( this->file_protocol->GetInfo( this->file_protocol, &gEfiFileInfoGuid, &info_size, file_info ) ) ) {
#ifdef DEBUG
        print( u"[ERROR] Failed to Get File Info.\n" );
#endif
    }
    // 为计算文件所占用的页数量并为其分配页
    auto file_page_size { ( file_info->FileSize >> 12 ) + 1 };
    EFI_PHYSICAL_ADDRESS file_buffer_address { };
    if ( EFI_ERROR( gBS->AllocatePages( AllocateAnyPages, EfiLoaderData, file_page_size,
                                        &file_buffer_address ) ) ) {
#ifdef DEBUG
        print( u"[ERROR] Failed to AllocatePages for File.\n" );
#endif
        while ( true )
            ;
    }
    auto read_size { file_info->FileSize };
    // 读取内容

    if ( EFI_ERROR( this->file_protocol->Read( this->file_protocol, &read_size, reinterpret_cast< VOID * >( file_buffer_address ) ) ) ) {
#ifdef DEBUG
        print( u"[ERROR] Failed to Read File.\n" );
#endif
        while ( true )
            ;
    }
    delete[] file_info;
    file_base = file_buffer_address;
    return file_base;
}
FileService::~FileService( VOID ) noexcept {
    this->file_protocol->Close( this->file_protocol );
    // 关闭这个文件
}
}     // namespace QuantumNEC::Boot