#pragma once
#include "Socket.h"
#include "Channel.h"
#include "EventLoop.h"
#include <memory>
using namespace std;

/*
2024/10/20: 现在eventloop的框架已经写好了，eventloop已经实现了，可以把channel添加到epoll模型中
epoll会进行事件循环，然后把有事件发生的channel返回，然后调用channel处理事件的函数，处理事件
目前就想到下面这几个事还没干
1、现在还没有channel被添加到事件循环中
2、accept函数返回的fd也没有被转换成channel
3、也没有被加入事件循环中
*/

class Acceptor
{
private:
    EventLoop *loop_; /* 暂时先用普通指针 */
    Socket servSock_;   /* Acceptor在ReActor模型中只有一个，使用栈内存 */
    Channel acceptChannel_;    /* Acceptor在ReActor模型中只有一个，同理使用栈内存 */

/* Acceptor需要运行在一个事件循环中，那么问题来了，怎么知道Acceptor属于哪个事件循环呢？
那就是在Acceptor中存一个指针，指向它属于的事件循环
它只是被添加到某个事件循环中，它不对事件循环进行管理，所以它不能为时间循环申请内存、释放内存等
只能用一个指针标记一下，Acceptor属于哪个事件循环，仅此而已 */
    function<void(unique_ptr<Socket>)> newConnCallBack_;
public:
    Acceptor(const string &ip, const uint16_t port, EventLoop *loop);
    ~Acceptor();

/* 新客户端连接的处理函数，它就是完成Accept的工作 */
    void AcceptNewConn();
    void SetNewConnCallBack(function<void(unique_ptr<Socket>)> fn);
};