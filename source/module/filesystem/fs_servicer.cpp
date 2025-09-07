#include <module/filesystem/fs_servicer.hpp>

namespace Module {
auto filesystem_servicer( void ) -> void {
    __asm__( "int $0x80" );
    while ( true );
}
}     // namespace Module