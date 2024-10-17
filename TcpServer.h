/* TCP服务类，之前写的都是底层的类，谁来使用这些类来实现一个服务器的功能呢？那就是tcpserver类 */
#pragma once
#include <memory>
#include "EventLoop.h"
#include "Acceptor.h"
using namespace std;


class TcpServer
{
private:
    int timeout_;
    EventLoop mainLoop_; /* 暂时先用栈上的内存 */
    vector<unique_ptr<EventLoop>> subLoops_;
    Acceptor acceptor_;
public:
    TcpServer(const string& ip, uint16_t port, int timeout);
    ~TcpServer();
};
