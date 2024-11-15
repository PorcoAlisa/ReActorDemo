#pragma once
#include "TcpServer.h"
#include "ThreadPool.h"
using namespace std;

class EchoServer
{
private:
    TcpServer tcpServer_;
    ThreadPool workThreadPool_;
public:
    EchoServer(const string& ip, uint16_t port, uint32_t threadNum = 3, uint32_t workThreadNum_ = 3, int timeout = 80);
    ~EchoServer();

    void Start();
    void Stop();

    void AcceptNewConnInEchoServer(shared_ptr<Connection> conn);

    void HandleReadInEchoServer(Connection *conn, string &message);
    void HandleErrorInEchoServer(Connection *conn);
    void HandelCloseInEchoServer(Connection *conn);
    void HandleSendFinishInEchoServer(Connection *conn);

    void MessageProc(Connection *conn, string &message);
};