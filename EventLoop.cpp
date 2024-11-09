#include "EventLoop.h"
#include "Connection.h"
#include <sys/syscall.h>

EventLoop::EventLoop(bool mainLoop, int timeout):epoll_(new Epoll), stop_(false), timeout_(timeout), mainLoop_(mainLoop)
{
    if (mainLoop_) {
        printf("threadid = %d, mainLoop constructor: timeout_ = %d\n", syscall(SYS_gettid), timeout_);
    } else {
        printf("threadid = %d, slaveLoop constructor: timeout_ = %d\n", syscall(SYS_gettid), timeout_);
    }
}

EventLoop::~EventLoop()
{
    if (mainLoop_) {
        printf("threadid = %d, mainLoop destructor\n", syscall(SYS_gettid));
    } else {
        printf("threadid = %d, slaveLoop destructor\n", syscall(SYS_gettid));
    }
}

void EventLoop::RunLoop()
{
    /* 循环需要有非异常状态下的退出机制，因此设计stop_ */
    /* 思考stop需不需用原子类型，首先stop_需要被多个线程操作
    stop操作是通过信号来完成非异常状态下的退出的
    所以stop实际上是由主线程来执行的，而时间循环线程与主线程不一定是同一个线程，这里为了安全，采用原子类型变量 */
    if (mainLoop_) {
        printf("threadid = %d, mainLoop start runing\n", syscall(SYS_gettid));
    } else {
        printf("threadid = %d, slaveLoop start running\n", syscall(SYS_gettid));
    }
    while (!stop_) {
        vector<Channel *> channels;
        channels = epoll_->EpollWait(timeout_); /* TODO: 这里会发生拷贝，后续考虑优化 */
        if (channels.size() == 0) {
            /* size为0说明超时 */
            /* 那么接下来应该执行超时的处理，但是超时的处理不能放在eventloop里面
            超时怎么处理是业务关心的事情，eventloop虽然在epoll类的上层，但仍然属于底层类，提供事件循环，并执行相应处理的框架
            业务可能随时变动，但是底层类和框架不能随时变动，所以这里需要采用回调函数的形式 */
            if (TimeOutCallBack_ != NULL) {
                TimeOutCallBack_(this); /* this指针传出去，告诉外层，哪个事件循环超时了 */
            }
        } else {
            /* 说明有事件发生 */
            /* 同理，回调事件的处理函数 */
            for (auto & ch:channels) {
                /* 每个Channel有事件发生，必须交由每个channel自己来处理，需要给channel类添加处理事件的函数 */
                /* 但是channel也是底层类，channel不关心业务到底怎么处理事件，它也不知道怎么处理事件
                它依然是采用回调函数的方法，只是提供一个接口给业务注册，然后提供一个调用业务注册函数来执行的底层框架，将业务和底层框架剥离开来 */
                ch->HandleEvents();
            }
        }
    }
}

void EventLoop::StopLoop()
{
    stop_ = true;
    /* 这里还没完成，stop_设置为true之后，RunLoop里面很可能还卡在epoll_wait这一步，所以需要有个东西把eventloop从epollwait中唤醒 */
}

void EventLoop::SetTimeOutCallBackFunc(function<void(EventLoop *)> fn)
{
    TimeOutCallBack_ = fn;
}

void EventLoop::UpdateChannel(Channel *ch)
{
    epoll_->UpdateChannel(ch);
}

void EventLoop::RemoveChannel(Channel *ch)
{
    epoll_->RemoveChannel(ch);
}

void EventLoop::AddConnToConns(shared_ptr<Connection>& conn)
{
    lock_guard<mutex> lg(mutex_);
    conns_[conn->Fd()] = conn;
}