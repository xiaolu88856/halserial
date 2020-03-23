#ifndef _COMMON_H
#define _COMMON_H

#include <stdio.h>  
#include <string.h>  
#include <stdint.h>
#include <stdlib.h>

#include <pthread.h>

#define NULL ((void *)0)

typedef enum
{
    DTU_MODEL_NULL = 0,
    DTU_NISE_R2E1,
    DTU_NISE_R4E2,
    DTU_NISE_R8E1,
    DTU_NISE_R8E2,
    DTU_NISE_R16E2,
    DTU_S20_R4E2,
    DTU_S20_R8E2,
    DTU_PC_LINUX,
    DTU_PC_WIN
}dtu_model_t;

typedef enum
{
    DTU_COM1 = 0,
    DTU_COM2,
    DTU_COM3,
    DTU_COM4,
    DTU_COM5,
    DTU_COM6,
    DTU_COM7,
    DTU_COM8,
    DTU_COM9,
    DTU_COM10,
    DTU_COM11,
    DTU_COM12,
    DTU_COM13,
    DTU_COM14,
    DTU_COM15,
    DTU_COM16
}dtu_com_t;

typedef enum
{
    FORMAT_N81 = 0,
    FORMAT_E81,
    FORMAT_O81,
    FORMAT_N82
}format_t;

typedef struct 
{
    pthread_attr_t pthread_attr;
}g_param_t;

extern g_param_t g_param;

#endif