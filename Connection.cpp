#include "Connection.h"

/* Channel必须在eventloop中运行，channel类中需要添加eventloop成员变量，标记channel所属的eventloop */
Connection::Connection(EventLoop *loop, unique_ptr<Socket> clientSock)
    :loop_(loop), clientSock_(move(clientSock)), clientChannel_(new Channel(loop_, clientSock_->Fd()))
{
    clientChannel_->SetReadCallBack(bind(&Connection::HandleReadEvent, this)); /* 把Connection的this指针传入，这样在channel类中就知道是哪个Connection发生了read事件 */
}

Connection::~Connection()
{

}

void Connection::HandleReadEvent()
{

}

int Connection::Fd() const
{
    return clientSock_->Fd();
}