#include "Channel.h"
#include "EventLoop.h"

Channel::Channel(EventLoop *loop, int fd):loop_(loop), fd_(fd)
{

}

Channel::~Channel()
{
    
}

bool Channel::InEpoll()
{
    return inEpoll_;
}

uint32_t Channel::Events()
{
    return events_;
}

int Channel::Fd()
{
    return fd_;
}

void Channel::SetInEpoll(bool inEpoll)
{
    inEpoll_ = inEpoll;
}

void Channel::SetHappenedEvents(uint32_t happenedEvents)
{
    happenedEvents_ = happenedEvents;
}

/* 函数名叫handelevents，那handle什么events呢，handle epollwait的时候，监测到发生
并通过SetHappenedEvents写入happenedEvents_的事件 */
void Channel::HandleEvents()
{
    if (happenedEvents_ & EPOLLRDHUP) { /* 说明客户端已关闭连接 */
        if(closeCallBack_ != NULL) {
            closeCallBack_();
        } else {
            printf("Channel::HandleEvents() error closeCallBack_ NULL\n");
        }
    } else if (happenedEvents_ & (EPOLLIN|EPOLLPRI)) { /* 接收缓冲区中有数据可以读 */
        if (readCallBack_ != NULL) {
            readCallBack_();
        } else {
            printf("Channel::HandleEvents() error readCallBack_ NULL\n");
        }
    } else if (happenedEvents_ & EPOLLOUT) { /* 有数据需要写 */
        if (writeCallBack_ != NULL) {
            writeCallBack_();
        } else {
            printf("Channel::HandleEvents() error writeCallBack_ NULL\n");
        }
    } else {
        if (errorCallBack_ != NULL) {
            errorCallBack_();
        } else {
            printf("Channel::HandleEvents() error errorCallBack_ NULL\n");
        }
    }
}

void Channel::SetCloseCallBack(function<void()> fn)
{
    closeCallBack_ = fn;
}

void Channel::SetReadCallBack(function<void()> fn)
{
    readCallBack_ = fn;
}

void Channel::SetWriteCallBack(function<void()> fn)
{
    writeCallBack_ = fn;
}

void Channel::SetErrorCallBack(function<void()> fn)
{
    errorCallBack_ = fn;
}

void Channel::EnableReading()
{
    events_ |= EPOLLET;
    /* 需要在loop_中更新channel */
    loop_->UpdateChannel(this);
}

void Channel::EnableWriting()
{
    events_ |= EPOLLOUT;
    loop_->UpdateChannel(this);
}

void Channel::RemoveFromEpoll()
{
    loop_->RemoveChannel(this);
}

void Channel::UseEt()
{
    events_ = events_|EPOLLET; /* 设置使用边缘触发模式 */
}

void Channel::DisableWriting()
{
    events_ &= ~EPOLLOUT;
    loop_->UpdateChannel(this);
}