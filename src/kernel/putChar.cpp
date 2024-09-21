#include "include/printOut.h"
namespace Kernel
{
    VOID putChar
    (
        uint32_t* FB,
        int32_t Xsize,
        int32_t X,
        int32_t Y,
        uint32_t FRcolor,
        uint32_t BKcolor,
        uint8_t Font
    )
    {
        INT i = 0;
        INT j = 0;
        uint32_t* Address = nullptr;
        CHAR* Font_ptr = Ascii[Font];
        int32_t testVal = 0;
        for (i = 0; i < 16; ++i)
        {
            Address = FB + Xsize * (Y + i) + X;
            testVal = 0x100;
            for (j = 0; j < 8; ++j)
            {
                testVal = testVal >> 1;
                if (*Font_ptr & testVal) {
                    *Address = FRcolor;
                }
                else {
                    *Address = BKcolor;
                }
                ++Address;
            }
            ++Font_ptr;
        }
    }
}