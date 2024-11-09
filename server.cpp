#include <iostream>
#include <signal.h>
#include "EchoServer.h"
#include <sys/syscall.h>

EchoServer *echoServer;

void Stop(int sig)
{
    printf("threadid = %d, receive sig = %d\n", syscall(SYS_gettid), sig);
    echoServer->Stop();
    printf("threadid = %d, echoServer has stoped. \n", syscall(SYS_gettid));
    delete echoServer;
    printf("threadid = %d, delete echoServer. \n", syscall(SYS_gettid));
    exit(0);
}

int main(int argc, char *argv[])
{
    if (argc != 3) {
        printf("usage: ./server ip port\n");
        printf("example: ./server 0.0.0.0 39001\n");
        return -1;
    }

    signal(SIGTERM, Stop);
    signal(SIGINT, Stop);

    echoServer = new EchoServer(argv[1], atoi(argv[2]));
    echoServer->Start();

    return 0;
}