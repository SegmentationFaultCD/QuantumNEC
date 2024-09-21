
#ifndef _IO_H_
#define _IO_H_
#include "include/stdint.h"
#include "include/stdio.h"
namespace Kernel
{
    /*  */

    inline uint8_t io_in8(uint16_t port);

    /*  */

    inline uint32_t io_in32(uint16_t port);

    /*  */

    inline VOID io_out8(uint16_t port, uint8_t value);

    /*  */

    inline VOID io_out32(uint16_t port, uint32_t value);
}
#endif /* !_IO_H_ */