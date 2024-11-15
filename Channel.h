#pragma once
#include <stdint.h>
#include <functional>
#include <sys/epoll.h>
using namespace std;

class EventLoop;

class Channel
{
private:
    EventLoop *loop_;
    int fd_;
    uint32_t events_ = 0; /* 在添加到epoll模型监视的时候，需要知道监视什么events */
    uint32_t happenedEvents_ = 0; /* epoll模型监测到事件发生后，记录发生了什么事件 */
    bool inEpoll_ = false; /* 添加到epoll模型之后，可能会对监视的事件修改，此时需要区分channel是否已被添加到Epoll（add或mod） */

 /* 其实这些是对外的接口，告诉外部，如果发生了关闭事件，你需要我做什么，可以通过这些回调函数来指定 */
    function<void()> closeCallBack_;
    function<void()> readCallBack_;
    function<void()> writeCallBack_;
    function<void()> errorCallBack_;
public:
    Channel(EventLoop *loop, int fd);
    ~Channel();
    bool InEpoll();
    uint32_t Events();
    int Fd();
    void SetInEpoll(bool inEpoll);
    void HandleEvents();
    void SetHappenedEvents(uint32_t happenedEvents);
    void SetCloseCallBack(function<void()> fn);
    void SetReadCallBack(function<void()> fn);
    void SetWriteCallBack(function<void()> fn);
    void SetErrorCallBack(function<void()> fn);
    void EnableReading();
    void EnableWriting();
    void DisableWriting();
    void RemoveFromEpoll();
    void UseEt(); /* 设置边缘触发模式 */
};