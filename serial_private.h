#ifndef _LIB_SERIAL_PRIVATE_H
#define _LIB_SERIAL_PRIVATE_H

#include <termios.h>
#include "common.h"

typedef struct
{
    struct termios old_tios;

    dtu_model_t model; // DTU型号
    dtu_com_t com;

    int gpio_fd; // RS485 GPIO设备号
    uint8_t is_rs485_mode;
    uint8_t rs485_io_num;
    int onebyte_time;
    int lastbyte_delay;
}serial_private_t;

#endif

