/* TCP服务类，之前写的都是底层的类，谁来使用这些类来实现一个服务器的功能呢？那就是tcpserver类 */
#pragma once
#include <memory>
#include <functional>
#include <map>
#include <mutex>
#include "EventLoop.h"
#include "Acceptor.h"
#include "Connection.h"
#include "ThreadPool.h"
using namespace std;


class TcpServer
{
private:
    int timeout_;
    uint32_t threadNum_;
    EventLoop mainLoop_; /* 暂时先用栈上的内存 */
    vector<unique_ptr<EventLoop>> subLoops_;
    Acceptor acceptor_;
    ThreadPool threadPool_;
    map<int, shared_ptr<Connection>> conns_; /* 先暂时用unique_ptr，等后面看看到底什么原因，必须换成shared_ptr */
    mutex mutex_;
    function<void(shared_ptr<Connection>)> newConnCallBackInTcpServer_;
/* 现在在acceptor类里面，已经完成了accept，并且创建了新的socket，利用新的socket创建了新的Socket，
使用的是unique_ptr，并且通过已经传入内部的回调函数，等待外部设定回调函数 */
public:
    TcpServer(const string& ip, uint16_t port, int timeout, uint32_t threadNum);
    ~TcpServer();
    void AcceptNewConnByTcpServer(unique_ptr<Socket> clientSock);
    void OnEpollTimeout(EventLoop *loop);
    void RunEventLoop();
    void StopEventLoop();
    void SetNewConnCallBackInTcpServer(function<void(shared_ptr<Connection>)> fn);
};
