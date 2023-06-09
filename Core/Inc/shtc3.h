#ifndef SHTC3_H
#define SHTC3_H

#include "stm32f1xx_hal_i2c.h"

#define SHTC3_ADDRESS_WRITE 0xE0
#define SHTC3_ADDRESS_READ 0xE1
#define SHTC3_COMMAND_WAKEUP 0x3517
#define SHTC3_COMMAND_MEASURING 0x5C24

#endif