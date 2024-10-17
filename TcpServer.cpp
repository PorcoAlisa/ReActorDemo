#include "TcpServer.h"

TcpServer::TcpServer(const string& ip, uint16_t port, int timeout)
    :timeout_(timeout), mainLoop_(timeout_), acceptor_(ip, port, &mainLoop_)
{
    mainLoop_.SetTimeOutCallBackFunc(bind(&TcpServer::OnEpollTimeout, this, placeholders::_1));
    acceptor_.SetNewConnCallBack(bind(&TcpServer::AcceptNewConnByTcpServer, this, placeholders::_1));
}

TcpServer::~TcpServer()
{

}

void TcpServer::AcceptNewConnByTcpServer(unique_ptr<Socket> clientSock)
{
    /* 需要有一个类来管理连接，就像Acceptor一样 */

}

void TcpServer::OnEpollTimeout(EventLoop *loop)
{

}

void TcpServer::RunEventLoop()
{
    mainLoop_.RunLoop();
}

void TcpServer::StopEventLoop()
{
    mainLoop_.StopLoop();
}