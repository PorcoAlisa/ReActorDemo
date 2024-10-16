#pragma once
#include <sys/epoll.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <strings.h>
#include <errno.h>
#include "Channel.h"
using namespace std;

/*
Epoll类的思路：
1、Epoll类的作用，Epoll类是实现epoll模型，即把可以监视事件发生的fd添加到epoll模型中
2、考虑到epoll类仅返回fd数据量太少，所以实际上让epoll在有事件发生的时候返回channel
3、Epoll要对外提供方法，将channel更新到epoll模型中，将channel从epoll模型中移除、以及实现epoll_wait
*/

#define MAX_EVENTS 100

/* 
为什么epoll类不和eventloop类合并？
我理解实际上可以合并，但是从架构上来说不够合理
epoll类主要目的是封装epoll的功能，使得epoll对外的表现像一个类，而不是面向过程的编程模式

epoll的功能是什么？
epoll的功能就是监视fd事件的发生
1、对于发生了事件的fd，将事件的fd吐回，告诉你，哪些fd发生了事件
2、告诉你发生了什么事件

struct epoll_event
{
  uint32_t events;	// Epoll events
  epoll_data_t data;	// User data variable
} __EPOLL_PACKED;

结合epoll_event的结构体可以说明epoll的功能
data主要通过data.ptr指明哪些fd发生了事件，因为data.ptr指向channel，channel与fd 一一对应
events指明，发生的是什么事件

所以epoll类只实现这两个功能，至于发生了事情之后，你要不要处理，怎么处理，epoll类一概不关心
epoll类只负责将这些返回给上层类，由上层类关心

那么epoll类写完之后，就要思考，现在epoll类已经实现了将发生事件的fd返回，并指明发生了什么事件，
那么理所当然地，下一步的代码就要考虑如何处理这些事件

所以下一步，添加一个类，实现对于事件的处理，这个类给它起名叫eventloop类，因为它要不断循环进行epoll_wait
对于epoll_wait返回的事件，不断地进行循环处理
*/

class Epoll
{
private:
    int epollFd_ = -1;
    epoll_event events_[MAX_EVENTS];
public:
    Epoll();
    ~Epoll();

    void UpdateChannel(Channel *ch);
    void RemoveChannel(Channel *ch);
    vector<Channel *> EpollWait(int timeout);
};