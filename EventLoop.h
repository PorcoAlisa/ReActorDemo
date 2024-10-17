#pragma once
#include "Epoll.h"
#include <atomic>
#include <functional>
using namespace std;

/*
EventLoop类中实现对于事件的处理
*/

class EventLoop
{
private:
    atomic_bool stop_;
    Epoll epoll_;
    int timeout_;
    /* 这里考虑设计超时回调函数
    超时回调函数必须传入this指针作为入参，否则业务注册函数时，将失去EventLoop *入参，业务无法得知是哪个时间循环类超时 */
    function<void(EventLoop *)> TimeOutCallBack_;
public:
    EventLoop(int timeout = 80);
    ~EventLoop();
    void RunLoop();
    void StopLoop();
    void SetTimeOutCallBackFunc(function<void(EventLoop *)> fn);
};