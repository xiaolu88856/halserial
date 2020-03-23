#ifndef _TCP_SRV_H
#define _TCP_SRV_H

#include "common.h"

extern void *tcp_proc(void *param);
extern int tcp_write(int fd,uint8_t* msg,int len);

#endif
