#pragma once
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string>
#include "InetAddress.h"

using namespace std;

class Socket
{
private:
    const int fd_;
    string ip_;
    uint16_t port_;

public:
    Socket(int fd);
    ~Socket();
    void Bind(const InetAddress &servaddr);
    void Listen(int n); /* n代表队列长度 */
    int Accept(InetAddress &clientAddr);
    /* TODO: 搞清楚每个选项的作用 */
    void SetSockOpt(bool noDelaySw, bool reUseAddrSw, bool reUsePortSw, bool keepAliveSw);
};