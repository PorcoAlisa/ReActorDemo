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