#pragma once
#include <memory>
#include "Socket.h"
#include "Channel.h"
#include "EventLoop.h"
using namespace std;

class Connection
{
private:
    /* 首先connection是用来管理accept得到的socket，所以必须有socket变量，
    其次，connection需要运行在一个事件循环中，通过事件循环监视发生的事件，所以也需要一个事件循环变量
    （这时候的事件循环类还没有把channel加入或更新到epoll模型的功能，现在此功能还在epoll模型中
    但是epoll模型的功能已经被封装在eventloop中，其所有功能由eventloop代理，所以还需要在eventloop类中实现更新channel的功能 */
    /* 有事件循环，就需要用于事件循环的channel */
    EventLoop* loop_;
    unique_ptr<Socket> clientSock_;
    unique_ptr<Channel> clientChannel_;

public:
    Connection(EventLoop *loop, unique_ptr<Socket> clientSock);
    ~Connection();
};

Connection::~Connection()
{
}
