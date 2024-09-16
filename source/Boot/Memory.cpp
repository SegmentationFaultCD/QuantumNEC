#include <Boot/memory.hpp>
#include <Library/BaseMemoryLib.h>
#include <Boot/output.hpp>
namespace QuantumNEC::Boot {
MemoryService::MemoryService( VOID ) noexcept {
    // 设置config
    this->Buffer = NULL;
    this->DescriptorSize = 0;
    this->DescriptorVersion = 0;
    this->MemoryKey = 0;
    this->MemoryCount = 0;
    this->MemorySize = 0;
    // 获取memory map
    while ( gBS->GetMemoryMap( &this->MemorySize, this->Buffer, &this->MemoryKey, &this->DescriptorSize, &this->DescriptorVersion ) == EFI_BUFFER_TOO_SMALL ) {
        if ( this->Buffer ) {
            delete this->Buffer;
            this->Buffer = NULL;
        }
        this->Buffer = new EFI_MEMORY_DESCRIPTOR[ this->MemorySize ];     // 为memory map分配内存块
        if ( !this->Buffer ) {
#ifdef DEBUG
            print( u"[SUCCESS] Failed to get Memory Map.\n" );
#endif
            while ( true )
                ;
        }
    }
    // 获得内存块数量
    this->MemoryCount = MemorySize / DescriptorSize;
#ifdef DEBUG
    print( u"[SUCCESS] Get Memory Map.\n" );
#endif
}

}     // namespace QuantumNEC::Boot