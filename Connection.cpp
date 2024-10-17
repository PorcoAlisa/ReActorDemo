#include "Connection.h"

/* Channel必须在eventloop中运行，channel类中需要添加eventloop成员变量，标记channel所属的eventloop */
Connection::Connection(EventLoop *loop, unique_ptr<Socket> clientSock)
    :loop_(loop), clientSock_(move(clientSock)), clientChannel_(new Channel(loop_, clientSock_->Fd()))
{

}