#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

int main(int argc, char *argv[])
{
    if (argc != 3) {
        printf("usage: ./client ip port\n");
        printf("example: ./client 192.168.50.132 5085\n");
        return -1;
    }

    int clientFd = socket(AF_INET, SOCK_STREAM, 0); /* Step1: 创建客户端socket */
    if (clientFd < 0) {
        printf("socket() failed. \n");
        return -1;
    }

    struct sockaddr_in servAddr;
    memset(&servAddr, 0, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons(atoi(argv[2]));    /* 通过sockaddr_in结构体指定服务器端口 */
    servAddr.sin_addr.s_addr = inet_addr(argv[1]);    /* 通过sockaddr_in结构体指定服务器ip */

    char buf[1020];
    /* 将sockaddr_in结构体转换成sockaddr结构体传入connect函数，即指定服务端的端口与ip地址，并发起连接 */
    if (connect(clientFd, (struct sockaddr *)&servAddr, sizeof(servAddr)) != 0) {
        printf("connect(%s:%s) failed.\n", argv[1], argv[2]);
        close(clientFd);
        return -1;
    }

    printf("connect ok. \n");
    printf("开始时间：%d\n", time(0));

    for (int i = 0; i < 10; i++) {
        memset(buf, 0, sizeof(buf));
        sprintf(buf, "这是第%d条消息", i);

        char tmpBuf[1024];
        memset(tmpBuf, 0, sizeof(tmpBuf));

        int len = strlen(buf);
        memcpy(tmpBuf, &len, 4);
        memcpy(tmpBuf + 4, buf, len);

        send(clientFd, tmpBuf, len + 4, 0);
        len = 0;
        recv(clientFd, &len, 4, 0);
        memset(buf, 0, sizeof(buf));
        recv(clientFd, buf, len, 0);
    }

    printf("结束时间：%d\n", time(0));
}