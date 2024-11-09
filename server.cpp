#include <iostream>
#include "EchoServer.h"

EchoServer *echoServer;

void Stop(int sig)
{
    echoServer->Stop();
    delete echoServer;
    exit(0);
}

int main(int argc, char *argv[])
{
    if (argc != 3) {
        printf("usage: ./server ip port\n");
        printf("example: ./server 162.251.95.39 5085\n");
        return -1;
    }

    echoServer = new EchoServer(argv[1], atoi(argv[2]));
    echoServer->Start();

    return 0;
}