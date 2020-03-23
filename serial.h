#ifndef _SERIAL_H
#define _SERIAL_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util.h"

typedef struct 
{
    char device[16];  // 串口设备号，如:/dev/ttySP0
    int baud;         // 波特率，1200-115200
    uint8_t data_bit; // 数据位
    uint8_t stop_bit; // 停止位
    char parity;      // 奇偶检验位

    int s;      // 串口套接字
    void* msp;  // MCU Special Param,初始化接口参数
} serial_t;

// 创建串口类
// com      -- 端口号，COM1,COM2....COM16
// baud     -- 波特率，1200-115200
// parity   -- 奇偶检验位，'N','E','O'
// data_bit -- 数据位
// stop_bit -- 停止位
serial_t* serial_create(dtu_model_t model,const char *com,
                         int baud, char parity, int data_bit,
                         int stop_bit);

void serial_release(serial_t* serial);
// 打开串口
int serial_open(serial_t* serial);

void serial_close(serial_t* serial);

// 写串口数据
// msg     -- 缓存区
// msg_len -- 待写入数据长度
int serial_write(serial_t* serial, uint8_t *msg, int msg_len);

// 判断是否有数据可读
// timeout -- 读超时，单位ms
// return  -- 0超时， > 0 有数据 ， < 0,错误
// int serial_select(serial_t* serial,int timeout);

// 读取数据
// msg     -- 数据缓存
// msg_len -- 希望读取的数据长度
// return  -- 返回实际读取的长度
int serial_read(serial_t* serial, uint8_t *msg);

// 刷新串口数据
int serial_flush(serial_t* serial);
#endif

