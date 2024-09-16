#pragma once
#include <Boot/base.hpp>

namespace QuantumNEC::Boot {
class GraphicsService
{
private:
    struct BmpConfig
    {
        uint64_t size;
        uint64_t page_size;
        uint64_t width;
        uint64_t height;
        uint64_t offset;
        uint64_t pixel_start;
        uint64_t bits_per_px;
    };

private:
    EFI_PHYSICAL_ADDRESS FrameBufferBase;
    uint64_t FrameBufferSize;
    uint32_t HorizontalResolution;
    uint32_t VerticalResolution;
    uint32_t PixelsPerScanLine;

public:
    explicit GraphicsService( VOID ) noexcept;
    ~GraphicsService( VOID ) noexcept {
    }

public:
    auto display_logo( IN uchar_t *path ) noexcept -> VOID;
};

}     // namespace QuantumNEC::Boot