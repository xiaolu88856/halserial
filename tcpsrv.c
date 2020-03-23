#include <unistd.h>
#include <sys/types.h> /* basic system data types */
#include <sys/socket.h> /* basic socket definitions */
#include <netinet/in.h> /* sockaddr_in{} and other Internet defns */
#include <arpa/inet.h> /* inet(3) functions */
#include <sys/epoll.h> /* epoll function */
#include <fcntl.h> /* nonblocking */
#include <sys/resource.h> /*setrlimit */
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

#include "cfg.h"
#include "serial.h"

#define MAXEPOLLSIZE 2
#define MAXLINE 1024

int handle(int connfd,cfg_serial_t* cfg);

int setnonblocking(int sockfd)
{
    if (fcntl(sockfd, F_SETFL, fcntl(sockfd, F_GETFD, 0) | O_NONBLOCK) == -1)
    {
        return -1;
    }
    return 0;
}


void *tcp_proc(void *param)
{
    cfg_serial_t* cfg = (cfg_serial_t*)param;
    char* srvIp = "127.0.0.1";
    cfg->fd = -1;

    int listenq = 1024;
    int listenfd, connfd, kdpfd, nfds, n, nread, curfds, acceptCount = 0;
    struct sockaddr_in servaddr, cliaddr;
    socklen_t socklen = sizeof(struct sockaddr_in);
    struct epoll_event ev;
    struct epoll_event events[MAXEPOLLSIZE];
    struct rlimit rt;
    char buf[MAXLINE];

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(srvIp);
    servaddr.sin_port = htons(cfg->map_port);

    listenfd = socket(AF_INET, SOCK_STREAM, 0);

    if (listenfd == -1)
    {
        perror("can't create socket file");
        pthread_exit(NULL);
    }

    int opt = 1;
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    if (setnonblocking(listenfd) < 0)
    {
        perror("setnonblock error");
    }

    if (bind(listenfd, (struct sockaddr *)&servaddr, sizeof(struct sockaddr)) == -1)
    {
        perror("bind error");
        pthread_exit(NULL);
    }

    if (listen(listenfd, listenq) == -1)
    {
        perror("listen error");
        pthread_exit(NULL);
    }

    /*创建epoll句柄，把监听 socket 加入到 epoll 集合里 */
    kdpfd = epoll_create(MAXEPOLLSIZE);
    ev.events = EPOLLIN | EPOLLET;
    ev.data.fd = listenfd;
    if (epoll_ctl(kdpfd, EPOLL_CTL_ADD, listenfd, &ev) < 0)
    {
        fprintf(stderr, "epoll set insertion error: fd=%d\n", listenfd);
        pthread_exit(NULL);
    }

    curfds = 1;
    printf("epollserver startup,server:%s, port %d\n", srvIp,cfg->map_port);

    for (;;)
    {
        /* 等待有事件发生 */
        nfds = epoll_wait(kdpfd, events, curfds, -1);
        if (nfds == -1)
        {
            perror("epoll_wait");
            continue;
        }

        //DEBUG_PRINTF("events happen %d\n", nfds);
        //printf("events happen %d\n", nfds);

        /* 处理所有事件 */
        for (n = 0; n < nfds; ++n)
        {
            if (events[n].data.fd == listenfd)
            {
                connfd = accept(listenfd, (struct sockaddr *)&cliaddr, &socklen);

                if (connfd < 0)
                {
                    perror("accept error");
                    continue;
                }

                sprintf(buf, "accept form %s:%d\n", inet_ntoa(cliaddr.sin_addr), cliaddr.sin_port);
                printf("%d:%s", ++acceptCount, buf);

                if (curfds >= MAXEPOLLSIZE)
                {
                    fprintf(stderr, "too many connection, more than %d\n", MAXEPOLLSIZE);
                    close(connfd);
                    continue;
                }

                if (setnonblocking(connfd) < 0)
                {
                    perror("setnonblocking error");
                }

                ev.events = EPOLLIN | EPOLLET;
                ev.data.fd = connfd;

                if (epoll_ctl(kdpfd, EPOLL_CTL_ADD, connfd, &ev) < 0)
                {
                    fprintf(stderr, "add socket '%d' to epoll failed: %s\n", connfd, strerror(errno));
                    pthread_exit(NULL);
                }

                curfds++;
                continue;
            }

            // 处理客户端请求
            if (handle(events[n].data.fd,cfg) < 0)
            {
                epoll_ctl(kdpfd, EPOLL_CTL_DEL, events[n].data.fd, &ev);
                curfds--;
            }
        }
    }

    close(listenfd);    
    pthread_exit(NULL);
}

int handle(int connfd,cfg_serial_t* cfg)
{
    int nread;
    char req[MAXLINE];
    cfg->fd = connfd;

    nread = read(connfd, req, MAXLINE); //读取客户端socket流

    if (nread == 0)
    {
        cfg->fd = -1;
        printf("client %d close the connection\n",connfd);
        close(connfd);
        return -1;
    }
    else if (nread < 0)
    {
        cfg->fd = -1;
        perror("read error");
        close(connfd);
        return -1;
    }
    else
    {
        serial_write((serial_t*)cfg->serial,req,nread);
    }

    return 0;
}


int tcp_write(int fd,uint8_t* msg,int len)
{
    return write(fd,msg,len);
}