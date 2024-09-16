#pragma once
#include <Boot/base.hpp>
namespace QuantumNEC::Boot {
class FontLoaderService
{
public:
    explicit FontLoaderService( IN uchar_t *path ) noexcept;
    ~FontLoaderService( VOID ) noexcept {
    }

private:
    VOID *font_frame;
};

}     // namespace QuantumNEC::Boot