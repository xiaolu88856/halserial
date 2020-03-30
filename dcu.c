#include "serial.h"
#include "cfg.h"
#include "tcpsrv.h"

pthread_t serial_thread[16];
pthread_t tcp_thread[16];

void *serial_proc(void *param)
{
    serial_t* serial;
    cfg_serial_t* cfg = (cfg_serial_t*)param;
    switch(cfg->format)
    {
        case FORMAT_N81:serial = serial_create(dtu_model,cfg->com,cfg->baudrate,'N',8,1);break;
        case FORMAT_E81:serial = serial_create(dtu_model,cfg->com,cfg->baudrate,'E',8,1);break;
        case FORMAT_O81:serial = serial_create(dtu_model,cfg->com,cfg->baudrate,'0',8,1);break;
        case FORMAT_N82:serial = serial_create(dtu_model,cfg->com,cfg->baudrate,'N',8,2);break;
        default:serial = serial_create(dtu_model,cfg->com,cfg->baudrate,'N',8,1);break;
    }
    
    if(serial == NULL) pthread_exit(NULL); 

    int err = serial_open(serial);
    if(err < 0) pthread_exit(NULL); 

    cfg->serial = (void*)serial;

    while(1)
    {
        uint8_t msg[1024];
        int len = serial_read(serial,msg);

        if(len > 0)
        {
            // TCP服务器发送数据
            // printf("serial recv data:");
            // for(int i=0;i<len;i++)
            // {
            //     printf("%02x ",msg[i]);
            // }
            // printf("\n");

            if(cfg->fd != -1)
            {
                tcp_write(cfg->fd,msg,len);
            }
        }
    }
}

void dcu_init(void)
{
    int err = 0;
    int cnt = 0;
    int i = 0;
    for(i=0;i<MAX_SERIAL_NUM;i++)
    {
        if(cfg_serial[i].enable)
        {
            err = pthread_create(&serial_thread[cnt], &g_param.pthread_attr, serial_proc, &cfg_serial[i]);
            if (err)
            {
                printf("create %s thread failed!\n",cfg_serial[i].com);
                return;
            }
            printf("create %s thread!\n",cfg_serial[i].com);

            err = pthread_create(&tcp_thread[cnt], &g_param.pthread_attr, tcp_proc, &cfg_serial[i]);
            if (err)
            {
                printf("create tcp: 127.0.0.1:%d thread failed!\n",cfg_serial[i].map_port);
                return;
            }
            printf("create tcp: 127.0.0.1:%d thread!\n",cfg_serial[i].map_port);
        }

        cnt ++;
    }
}

void wait_for_dcu_done(void)
{
    int cnt = 0;
    int i=0;

    for(i=0;i<MAX_SERIAL_NUM;i++)
    {
        if(cfg_serial[i].enable)
        {
            pthread_join(serial_thread[cnt],NULL);
            pthread_join(tcp_thread[cnt],NULL);
        }
        cnt ++;
    }
}