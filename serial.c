// 
// 公共串口库，根据不同型号的通信管理机，提供串口收发、初始化等函数
//
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/ioctl.h>
#include "cJSON.h"

#include "serial_private.h"
#include "serial.h"
 

#define GPIO_U_IOCTL_BASE 'x'
#define GPIOC_OPS   _IOWR(GPIO_U_IOCTL_BASE,0,int)

static void gpio_set_txrx(int fd,int gpio_no, int state)
{
	unsigned long val;	
    // 默认最多8路使能控制
    if(gpio_no > 8)return;

	
    val = (!!state << 31) | gpio_no;	
    if(ioctl(fd,GPIOC_OPS,&val) < 0)
    {
        perror("ioctl gpio error");        
    }    
}


serial_t* serial_create(dtu_model_t model,const char *com,
                         int baud, char parity, int data_bit,
                         int stop_bit)
{
    serial_t* serial = (serial_t*)malloc(sizeof(serial_t));
    if(serial == NULL)
    {
        printf("malloc serial error!\n");
        return NULL;
    }

    serial->baud = baud;
    serial->parity = parity;
    serial->data_bit = data_bit;
    serial->stop_bit = stop_bit;

    serial->msp = (void*)malloc(sizeof(serial_private_t));
    if(serial->msp == NULL)
    {
        printf("malloc serial special param error!\n");
        return NULL;
    }
    serial_private_t* msp = (serial_private_t*)serial->msp;

    msp->is_rs485_mode = 0;
    msp->com = comstring_to_com(com);
    msp->model = model;
    if(msp->model == DTU_MODEL_NULL)
    {
        free(serial->msp);
        free(serial);
        return NULL;
    }

    memset(serial->device,0,sizeof(serial->device));
    switch(msp->model)
    {
        case DTU_NISE_R2E1:
            switch(msp->com)
            {
                case DTU_COM1:strcpy(serial->device,"/dev/ttySP2");break;
                case DTU_COM2:strcpy(serial->device,"/dev/ttySP1");break;
                default: strcpy(serial->device,"/dev/ttySP2");break;
            }
            break;
        case DTU_NISE_R4E2:
        case DTU_NISE_R8E2:
            switch(msp->com)
            {
                case DTU_COM1:strcpy(serial->device,"/dev/ttySP0");break;
                case DTU_COM2:strcpy(serial->device,"/dev/ttySP1");break;
                case DTU_COM3:strcpy(serial->device,"/dev/ttySP2");break;
                case DTU_COM4:strcpy(serial->device,"/dev/ttySP3");break;
                case DTU_COM5:strcpy(serial->device,"/dev/ttyUSB0");break;
                case DTU_COM6:strcpy(serial->device,"/dev/ttyUSB1");break;
                case DTU_COM7:strcpy(serial->device,"/dev/ttyUSB2");break;
                case DTU_COM8:strcpy(serial->device,"/dev/ttyUSB3");break;
                default: strcpy(serial->device,"/dev/ttySP0");break;
            }
            break;
        case DTU_NISE_R16E2:
            switch(msp->com)
            {
                case DTU_COM1:strcpy(serial->device,"/dev/ttyUSB0");break;
                case DTU_COM2:strcpy(serial->device,"/dev/ttyUSB1");break;
                case DTU_COM3:strcpy(serial->device,"/dev/ttyUSB2");break;
                case DTU_COM4:strcpy(serial->device,"/dev/ttyUSB3");break;
                case DTU_COM5:strcpy(serial->device,"/dev/ttyUSB4");break;
                case DTU_COM6:strcpy(serial->device,"/dev/ttyUSB5");break;
                case DTU_COM7:strcpy(serial->device,"/dev/ttyUSB6");break;
                case DTU_COM8:strcpy(serial->device,"/dev/ttyUSB7");break;
                case DTU_COM9:strcpy(serial->device,"/dev/ttyUSB8");break;
                case DTU_COM10:strcpy(serial->device,"/dev/ttyUSB9");break;
                case DTU_COM11:strcpy(serial->device,"/dev/ttyUSB10");break;
                case DTU_COM12:strcpy(serial->device,"/dev/ttyUSB11");break;
                case DTU_COM13:strcpy(serial->device,"/dev/ttyUSB12");break;
                case DTU_COM14:strcpy(serial->device,"/dev/ttyUSB13");break;
                case DTU_COM15:strcpy(serial->device,"/dev/ttyUSB14");break;
                case DTU_COM16:strcpy(serial->device,"/dev/ttyUSB15");break;
                default: strcpy(serial->device,"/dev/ttyUSB0");break;
            }
            break;        
        case DTU_S20_R4E2:
        case DTU_S20_R8E2:
            msp->is_rs485_mode = 1;
            switch(msp->com)
            {
                case DTU_COM4:
                    strcpy(serial->device,"/dev/ttymxc1");
                    msp->rs485_io_num = 0;
                    break;
                case DTU_COM3:
                    strcpy(serial->device,"/dev/ttymxc2");
                    msp->rs485_io_num = 1;
                    break;
                case DTU_COM2:
                    strcpy(serial->device,"/dev/ttymxc3");
                    msp->rs485_io_num = 2;
                    break;
                case DTU_COM1:
                    strcpy(serial->device,"/dev/ttymxc4");
                    msp->rs485_io_num = 3;
                    break;
                case DTU_COM8:
                    strcpy(serial->device,"/dev/ttyXRUSB1");
                    msp->rs485_io_num = 4;
                    break;
                case DTU_COM7:
                    strcpy(serial->device,"/dev/ttyXRUSB2");
                    msp->rs485_io_num = 5;
                    break;
                case DTU_COM6:
                    strcpy(serial->device,"/dev/ttyXRUSB0");
                    msp->rs485_io_num = 6;
                    break;
                case DTU_COM5:
                    strcpy(serial->device,"/dev/ttyXRUSB3");
                    msp->rs485_io_num = 7;
                    break;
                default: 
                    strcpy(serial->device,"/dev/ttymxc4");
                    msp->rs485_io_num = 3;
                    break;
            }
            break;
        case DTU_PC_LINUX:
            switch(msp->com)
            {
                // case DTU_COM1:strcpy(serial->device,"/dev/ttyUSB0");break;
                // case DTU_COM2:strcpy(serial->device,"/dev/ttyUSB1");break;
                // case DTU_COM3:strcpy(serial->device,"/dev/ttyUSB2");break;
                // case DTU_COM4:strcpy(serial->device,"/dev/ttyUSB3");break;
                // case DTU_COM5:strcpy(serial->device,"/dev/ttyUSB4");break;
                // case DTU_COM6:strcpy(serial->device,"/dev/ttyUSB5");break;
                // case DTU_COM7:strcpy(serial->device,"/dev/ttyUSB6");break;
                // case DTU_COM8:strcpy(serial->device,"/dev/ttyUSB7");break;
                // case DTU_COM9:strcpy(serial->device,"/dev/ttyUSB8");break;
                // case DTU_COM10:strcpy(serial->device,"/dev/ttyUSB9");break;
                // case DTU_COM11:strcpy(serial->device,"/dev/ttyUSB10");break;
                // case DTU_COM12:strcpy(serial->device,"/dev/ttyUSB11");break;
                // case DTU_COM13:strcpy(serial->device,"/dev/ttyUSB12");break;
                // case DTU_COM14:strcpy(serial->device,"/dev/ttyUSB13");break;
                // case DTU_COM15:strcpy(serial->device,"/dev/ttyUSB14");break;
                // case DTU_COM16:strcpy(serial->device,"/dev/ttyUSB15");break;
                // default: strcpy(serial->device,"/dev/ttyUSB0");break;
                case DTU_COM1:strcpy(serial->device,"/dev/ttyS1");break;
                case DTU_COM2:strcpy(serial->device,"/dev/ttyS2");break;
                case DTU_COM3:strcpy(serial->device,"/dev/ttyS3");break;
                case DTU_COM4:strcpy(serial->device,"/dev/ttyS4");break;
                case DTU_COM5:strcpy(serial->device,"/dev/ttyS5");break;
                case DTU_COM6:strcpy(serial->device,"/dev/ttyS6");break;
                case DTU_COM7:strcpy(serial->device,"/dev/ttyS7");break;
                case DTU_COM8:strcpy(serial->device,"/dev/ttyS8");break;
                case DTU_COM9:strcpy(serial->device,"/dev/ttyS9");break;
                case DTU_COM10:strcpy(serial->device,"/dev/ttyS10");break;
                case DTU_COM11:strcpy(serial->device,"/dev/ttyS11");break;
                case DTU_COM12:strcpy(serial->device,"/dev/ttyS12");break;
                case DTU_COM13:strcpy(serial->device,"/dev/ttyS13");break;
                case DTU_COM14:strcpy(serial->device,"/dev/ttyS14");break;
                case DTU_COM15:strcpy(serial->device,"/dev/ttyS15");break;
                case DTU_COM16:strcpy(serial->device,"/dev/ttyS16");break;
                default: strcpy(serial->device,"/dev/ttyS1");break;
            }
            break;
        case DTU_PC_WIN:
            switch(msp->com)
            {
                case DTU_COM1:strcpy(serial->device,"COM0");break;
                case DTU_COM2:strcpy(serial->device,"COM1");break;
                case DTU_COM3:strcpy(serial->device,"COM2");break;
                case DTU_COM4:strcpy(serial->device,"COM3");break;
                case DTU_COM5:strcpy(serial->device,"COM4");break;
                case DTU_COM6:strcpy(serial->device,"COM5");break;
                case DTU_COM7:strcpy(serial->device,"COM6");break;
                case DTU_COM8:strcpy(serial->device,"COM7");break;
                case DTU_COM9:strcpy(serial->device,"COM8");break;
                case DTU_COM10:strcpy(serial->device,"COM9");break;
                case DTU_COM11:strcpy(serial->device,"COM10");break;
                case DTU_COM12:strcpy(serial->device,"COM11");break;
                case DTU_COM13:strcpy(serial->device,"COM12");break;
                case DTU_COM14:strcpy(serial->device,"COM13");break;
                case DTU_COM15:strcpy(serial->device,"COM14");break;
                case DTU_COM16:strcpy(serial->device,"COM15");break;
                default: strcpy(serial->device,"COM0");break;
            }
            break;
        default: 
            printf("unsupport DTU type!\n");
            free(serial->msp);
            free(serial);
            serial = NULL;
            break;
    }
    msp->onebyte_time = 1000000 * (1 + data_bit + (parity == 'N' ? 0 : 1) + stop_bit) / baud;
    msp->lastbyte_delay = msp->onebyte_time;

    return serial;
}

void serial_release(serial_t* serial)
{
    if(serial != NULL)
    {
        if(serial->msp != NULL) free(serial->msp);
        free(serial);
    }
}

int serial_open(serial_t* serial)
{
    struct termios tios;
    speed_t speed;    
    serial->s = open(serial->device, O_RDWR | O_NOCTTY | O_NDELAY | O_EXCL);
    if (serial->s == -1) {
        printf("ERROR Can't open the device %s (%s)\n",
                serial->device, strerror(errno));
        return -1;
    }

    serial_private_t* msp = (serial_private_t*)serial->msp;

    /* Save */
    tcgetattr(serial->s, &(msp->old_tios));

    memset(&tios, 0, sizeof(struct termios));
    switch (serial->baud) {
    case 110:
        speed = B110;
        break;
    case 300:
        speed = B300;
        break;
    case 600:
        speed = B600;
        break;
    case 1200:
        speed = B1200;
        break;
    case 2400:
        speed = B2400;
        break;
    case 4800:
        speed = B4800;
        break;
    case 9600:
        speed = B9600;
        break;
    case 19200:
        speed = B19200;
        break;
    case 38400:
        speed = B38400;
        break;
    case 57600:
        speed = B57600;
        break;
    case 115200:
        speed = B115200;
        break;
    default:
        speed = B9600;
        printf("WARNING Unknown baud rate %d for %s (B9600 used)\n",
                serial->baud, serial->device);
    }

    /* Set the baud rate */
    if ((cfsetispeed(&tios, speed) < 0) ||
        (cfsetospeed(&tios, speed) < 0)) {
        close(serial->s);
        serial->s = -1;
        return -1;
    }    
    tios.c_cflag |= (CREAD | CLOCAL);
    tios.c_cflag &= ~CSIZE;
    switch (serial->data_bit) {
    case 5:
        tios.c_cflag |= CS5;
        break;
    case 6:
        tios.c_cflag |= CS6;
        break;
    case 7:
        tios.c_cflag |= CS7;
        break;
    case 8:
    default:
        tios.c_cflag |= CS8;
        break;
    }

    /* Stop bit (1 or 2) */
    if (serial->stop_bit == 1)
        tios.c_cflag &=~ CSTOPB;
    else /* 2 */
        tios.c_cflag |= CSTOPB;

    /* PARENB       Enable parity bit
       PARODD       Use odd parity instead of even */
    if (serial->parity == 'N') {
        /* None */
        tios.c_cflag &=~ PARENB;
    } else if (serial->parity == 'E') {
        /* Even */
        tios.c_cflag |= PARENB;
        tios.c_cflag &=~ PARODD;
    } else {
        /* Odd */
        tios.c_cflag |= PARENB;
        tios.c_cflag |= PARODD;
    }
    tios.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);

    if (serial->parity == 'N') {
        /* None */
        tios.c_iflag &= ~INPCK;
    } else {
        tios.c_iflag |= INPCK;
    }
    tios.c_iflag &= ~(IXON | IXOFF | IXANY);
    tios.c_oflag &=~ OPOST;
    tios.c_cc[VMIN] = 0;
    tios.c_cc[VTIME] = 0;

    if (tcsetattr(serial->s, TCSANOW, &tios) < 0) {
        close(serial->s);
        serial->s = -1;
        return -1;
    }

    if(msp->is_rs485_mode)
    {
        msp->gpio_fd = open("/dev/gpio",O_RDWR);
        if(msp->gpio_fd < 0)
        {
            printf("open gpio dev error");     
            return -1;
        }
        gpio_set_txrx(msp->gpio_fd,msp->rs485_io_num,1);
    }

    printf("serial open %s-%d,%c%d%d OK!\n",
        serial->device,serial->baud,serial->parity,serial->data_bit,serial->stop_bit);

    return 0;
}

void serial_close(serial_t* serial)
{
    serial_private_t* msp = (serial_private_t*)serial->msp;
    tcsetattr(serial->s, TCSANOW, &(msp->old_tios));
    close(serial->s);

    if(msp->is_rs485_mode)
    {
        close(msp->gpio_fd);
    }
}

int serial_write(serial_t* serial, uint8_t *msg, int msg_len)
{
    int size;
    serial_private_t* msp = (serial_private_t*)serial->msp;
    if(msp->is_rs485_mode)
    {
        gpio_set_txrx(msp->gpio_fd,msp->rs485_io_num,0);
        size = write(serial->s, msg, msg_len);
        usleep(msp->onebyte_time * msg_len + msp->lastbyte_delay);
        gpio_set_txrx(msp->gpio_fd,msp->rs485_io_num,1);
    }
    else 
    {
        size = write(serial->s, msg, msg_len);
    }
    return size;
}

// 阻塞接收
int serial_select(serial_t* serial)
{
    fd_set rfds;
    FD_ZERO(&rfds);
    FD_SET(serial->s, &rfds);

    int s_rc;
    while ((s_rc = select(serial->s+1, &rfds, NULL, NULL, NULL)) == -1) {
        if (errno == EINTR) 
        {
            printf("A non blocked signal was caught\n");
            FD_ZERO(&rfds);
            FD_SET(serial->s, &rfds);
        } 
        else 
        {
            return -1;
        }
    }

    if (s_rc == 0) {
        /* Timeout */
        errno = ETIMEDOUT;
        return -1;
    }

    return s_rc;
}

int serial_read(serial_t* serial, uint8_t *msg)
{
    int len = serial_select(serial);
    return read(serial->s, msg, len);
}

int serial_flush(serial_t* serial)
{
    return tcflush(serial->s, TCIOFLUSH);
}
