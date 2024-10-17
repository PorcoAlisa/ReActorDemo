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

int CreateNonBlocking();

class Socket
{
private:
    const int fd_;
    string ip_; /* 对于监听的socket，代表本地的ip，对于连接的socket，代表客户端的ip */
    uint16_t port_; /* 对于监听的socket，代表本地监听的端口，对于连接的socket，代表客户端的ip */

public:
/* 为什么socket在构造函数中只有fd？没有ip和port呢？
因为创建socket的时候，返回的就是fd，fd描述的就是一个socket，但是socket要实现通信，必须绑定ip和port
所以实际上socket有三个参数，fd、ip和port，那么ip和port什么时候指定呢，答案是绑定（bind）的时候
但这也只是创建一个socket的时候，需要去绑定ip和端口，而实际上accept的时候，返回一个新的socket，
这个socket的ip和port与服务器监听的ip和port是完全相同的，只是客户端的ip与port不同
那么accept的时候，就得手动记录一下客户端socket的ip和地址了
 */
    Socket(int fd);
    ~Socket();
    void Bind(const InetAddress &servaddr);
    void Listen(int n = 128); /* n代表队列长度 */
    int Accept(InetAddress &clientAddr);
    /* TODO: 搞清楚每个选项的作用 */
    void SetSockOpt(bool noDelaySw, bool reUseAddrSw, bool reUsePortSw, bool keepAliveSw);
    int Fd() const;
    void SetIpPort(const string& ip, uint16_t port);
};