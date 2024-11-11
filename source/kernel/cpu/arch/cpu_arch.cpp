#include <kernel/cpu/arch/cpu_arch.hpp>
#include <kernel/print.hpp>
QuantumNEC::Kernel::CpuArch::CpuArch( VOID ) noexcept {
    using enum DisplayColor;
    using namespace std;
    CpuidStatus status { };
    char_t      factoryName[ 17 ] { };

    status.mop = 0, status.sop = 0;

    this->cpuid( status );
    *reinterpret_cast< Ptr< uint32_t > >( &factoryName[ 0 ] ) = status.rbx;
    *reinterpret_cast< Ptr< uint32_t > >( &factoryName[ 4 ] ) = status.rdx;
    *reinterpret_cast< Ptr< uint32_t > >( &factoryName[ 8 ] ) = status.rcx;
    factoryName[ 12 ]                                         = '\0';

    println< ostream::HeadLevel::INFO >( "CPU device information is as follows:" );
    println< ostream::HeadLevel::INFO >( "{} {:x} {:x} {:x}         ", factoryName, status.rbx, status.rdx, status.rcx );

    // 获取处理器商标信息

    char_t factory_name_str[ 256 ] { };
    for ( uint32_t i { 0x80000002 }; i <= 0x80000004; i++ ) {
        status.mop = i, status.sop = 0;
        this->cpuid( status );
        *reinterpret_cast< Ptr< uint32_t > >( &factoryName[ 0 ] )  = status.rax;
        *reinterpret_cast< Ptr< uint32_t > >( &factoryName[ 4 ] )  = status.rbx;
        *reinterpret_cast< Ptr< uint32_t > >( &factoryName[ 8 ] )  = status.rcx;
        *reinterpret_cast< Ptr< uint32_t > >( &factoryName[ 12 ] ) = status.rdx;
        factoryName[ 16 ]                                          = '\0';
        std::strcat( factory_name_str, factoryName );
    }
    println< ostream::HeadLevel::INFO >( "{}", factory_name_str );
    // 获取处理器的版本信息
    status.mop = 1, status.sop = 0;
    this->cpuid( status );
    println< ostream::HeadLevel::INFO >(
        "Family Code -> {:x}, Extended Family -> {:x}, Model Number:{:x}, Extended Model -> {:x}, Processor Type -> {:x}, Stepping ID -> {:x}",
        ( status.rax >> 8 & 0xf ),       // 处理器商标信息引索值(IA-32处理器特有)
        ( status.rax >> 20 & 0xff ),     // CLFLUSH指令刷新的缓存行容量(单位8B)
        ( status.rax >> 4 & 0xf ),       // 处理器包内最大可寻址逻辑处理器ID
        ( status.rax >> 16 & 0xf ),      // 初始 APIC ID
        ( status.rax >> 12 & 0x3 ),
        ( status.rax & 0xf ) );     // 上面两个都是处理器支持的位功能信息
    //  获取线性/物理地址位宽
    status.mop = 0x80000008, status.sop = 0;
    this->cpuid( status );
    println< ostream::HeadLevel::INFO >( "Physical Address size -> {} | Linear Address size -> {}", ( status.rax & 0xff ), ( status.rax >> 8 & 0xff ) );
    // 获得处理器支持的最大基础功能号
    status.mop = 0, status.sop = 0;
    this->cpuid( status );
    println< ostream::HeadLevel::INFO >( "Max Basic Operation Code -> {:x}", status.rax );
    // 获得处理器支持的最大扩展功能号
    status.mop = 0x80000000, status.sop = 0;
    this->cpuid( status );
    println< ostream::HeadLevel::INFO >( "Max Extended Operation Code -> {:x}", status.rax );
}