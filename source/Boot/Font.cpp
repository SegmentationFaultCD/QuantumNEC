#include <Boot/font.hpp>
#include <Boot/file.hpp>
#include <Boot/output.hpp>
namespace QuantumNEC::Boot {
FontLoaderService::FontLoaderService( IN uchar_t *path ) noexcept {
    this->font_frame = reinterpret_cast< VOID * >( FileService { path }.read( ) );
#ifdef DEBUG
    print( u"[SUCCESS] The font file has been loaded.\n" );
#endif
}
}     // namespace QuantumNEC::Boot