#include <Include/Kernel/Print/printOut.h>

namespace Kernel_Types = QuantumNEC::Kernel::Types;
PRIVATE VOID putc(
    IN Kernel_Types::uint32_t far* FB,
    IN Kernel_Types::int32_t Xsize,
    IN Kernel_Types::int32_t X,
    IN Kernel_Types::int32_t Y,
    IN Kernel_Types::uint32_t FRcolor,
    IN Kernel_Types::uint32_t BKcolor,
    IN Kernel_Types::int8_t Font)
{
    Kernel_Types::int32_t i = 0, j = 0;
    Kernel_Types::uint32_t near* Address = NULL;
    Kernel_Types::CHAR near* FontPtr = NULL;
    Kernel_Types::int32_t testval = 0;
    FontPtr = QuantumNEC::Kernel::FONT::ASCII::FONT_ASCII[Font];

    for (i = 0; i < 16; i++)
    {
        Address = FB + Xsize * (Y + i) + X;
        testval = 0x100;
        for (j = 0; j < 8; j++)
        {
            testval = testval >> 1;
            if (*FontPtr & testval)
                *Address = FRcolor;
            else
                *Address = BKcolor;
            Address++;
        }
        FontPtr++;
    }
}
