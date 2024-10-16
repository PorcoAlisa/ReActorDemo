#pragma once
#include <stdint.h>

class Channel
{
private:
    int fd_;
    uint32_t events_ = 0; /* 在添加到epoll模型监视的时候，需要知道监视什么events */
    uint32_t happenedEvents_ = 0; /* epoll模型监测到事件发生后，记录发生了什么事件 */
    bool inEpoll_ = false; /* 添加到epoll模型之后，可能会对监视的事件修改，此时需要区分channel是否已被添加到Epoll（add或mod） */
public:
    Channel(int fd);
    ~Channel();
    bool InEpoll();
    uint32_t Events();
    int Fd();
    void SetInEpoll(bool inEpoll);
    void SetHappenedEvents(uint32_t happenedEvents);
};