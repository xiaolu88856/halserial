#ifndef _CFG_H
#define _CFG_H

#include "common.h"
#include "util.h"

#define MAX_SERIAL_NUM 16

typedef struct 
{
    int enable;           // 配置是否有效，0--无效，1--有效
    char com[16];   // 端口ID
    dtu_com_t com_id;
    int baudrate;
    format_t format;
    void* serial;

    int map_port;         // 映射到TCP服务器的端口号
    int fd;               // 服务器套接字
}cfg_serial_t;

extern cfg_serial_t cfg_serial[MAX_SERIAL_NUM];
extern dtu_model_t dtu_model;

extern void cfg_init(void);
#endif