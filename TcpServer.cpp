#include "TcpServer.h"

TcpServer::TcpServer(const string& ip, uint16_t port, int timeout)
    :timeout_(timeout), mainLoop_(timeout_), acceptor_(ip, port, &mainLoop_)
{

}

TcpServer::~TcpServer()
{

}