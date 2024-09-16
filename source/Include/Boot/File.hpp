#pragma once
#include <Boot/base.hpp>
namespace QuantumNEC::Boot {
class FileService
{
public:
    explicit FileService( IN uchar_t *path ) noexcept;
    ~FileService( VOID ) noexcept;

public:
    auto read( VOID ) -> uint64_t;

private:
    EFI_FILE_PROTOCOL *file_protocol;
    EFI_FILE_PROTOCOL *root { };
};
}     // namespace QuantumNEC::Boot