#include "EchoServer.h"

EchoServer::EchoServer(const string& ip, uint16_t port, uint32_t threadNum, int timeout):tcpServer_(ip, port, timeout, threadNum)
{
    tcpServer_.SetNewConnCallBackInTcpServer(bind(&EchoServer::AcceptNewConnInEchoServer, this, placeholders::_1));

    tcpServer_.SetReadCallBackInTcpServer(bind(&EchoServer::HandleReadInEchoServer, this, placeholders::_1, placeholders::_2));
    tcpServer_.SetErrorCallBackInTcpServer(bind(&EchoServer::HandleErrorInEchoServer, this, placeholders::_1));
    tcpServer_.SetCloseCallBackInTcpServer(bind(&EchoServer::HandelCloseInEchoServer, this, placeholders::_1));
    tcpServer_.SetSendFinishCallBackInTcpServer(bind(&EchoServer::HandleSendFinishInEchoServer, this, placeholders::_1));
}

EchoServer::~EchoServer()
{

}

void EchoServer::Start()
{
    tcpServer_.RunEventLoop();
}

void EchoServer::Stop()
{
    tcpServer_.StopEventLoop();
}

void EchoServer::AcceptNewConnInEchoServer(shared_ptr<Connection> conn)
{

}

/* 这里才是真正处理业务的位置 */
void EchoServer::HandleReadInEchoServer(Connection *conn, string &message)
{
    message = "reply: " + message;
    conn->SendInConn(message.data(), message.size());
}

void EchoServer::HandleErrorInEchoServer(Connection *conn)
{

}

void EchoServer::HandelCloseInEchoServer(Connection *conn)
{

}

void EchoServer::HandleSendFinishInEchoServer(Connection *conn)
{

}
