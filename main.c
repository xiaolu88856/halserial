// #include "serial.h"
#include <errno.h>
#include "cfg.h"
#include "dcu.h"

g_param_t g_param;

void g_param_init(void)
{
    pthread_attr_init(&g_param.pthread_attr);
    pthread_attr_setstacksize(&g_param.pthread_attr,512000); //512k
}

int main(int argc, char const *argv[])
{
    printf("start\n");
    cfg_init();
    dcu_init();

    wait_for_dcu_done();
    // serial_t* serial = serial_create(argv[1],9600,'N',8,1);
    // serial_open(serial);

    // char str[] = "libserial testing";
    // serial_write(serial,(uint8_t*)str,strlen(str));
    // printf("send %s\n",str);
    // serial_flush(serial);

    // int err = 0;
    // while(err <= 0)
    // {
    //     err = serial_select(serial,1000);

    //     if(err == -1 && errno == ETIMEDOUT)
    //     {
    //         printf("select timeout!\n");
    //         sleep(1);
    //     }
    // }

    // memset(str,0,strlen(str));
    // sleep(1);
    // int len = serial_read(serial,(uint8_t*)str,10);
    // printf("read: %s,len:%d\n",str,len);

    // serial_close(serial);
    // serial_release(serial);

    return 0;
}
