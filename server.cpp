#include <iostream>
#include "ThreadPool.h"

#include "Acceptor.h"

int main(int argc, char *argv[])
{
    if (argc != 3) {
        printf("usage: ./server ip port\n");
        printf("example: ./server 192.168.50.132 5085\n");
        return -1;
    }

    // Acceptor acceptor(argv[1], atoi(argv[2]));
    // InetAddress clientAddr;
    // acceptor.Accept(clientAddr);
}