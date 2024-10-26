#include "TcpServer.h"

TcpServer::TcpServer(const string& ip, uint16_t port, int timeout, uint32_t threadNum)
    :timeout_(timeout), threadNum_(threadNum), mainLoop_(timeout_), acceptor_(ip, port, &mainLoop_), threadPool_(threadNum_),
    newConnCallBackInTcpServer_(nullptr)
{
    mainLoop_.SetTimeOutCallBackFunc(bind(&TcpServer::OnEpollTimeout, this, placeholders::_1));
    acceptor_.SetNewConnCallBack(bind(&TcpServer::AcceptNewConnByTcpServer, this, placeholders::_1));

    for (uint32_t i = 0; i < threadNum_; i++) {
        subLoops_.emplace_back(new EventLoop(timeout_));
        subLoops_[i]->SetTimeOutCallBackFunc(bind(&TcpServer::OnEpollTimeout, this, placeholders::_1));
        threadPool_.AddTask(bind(&EventLoop::RunLoop, subLoops_[i].get()));
    }
}

TcpServer::~TcpServer()
{

}

void TcpServer::AcceptNewConnByTcpServer(unique_ptr<Socket> clientSock)
{
    /* 需要有一个类来管理连接，就像Acceptor一样 */
    shared_ptr<Connection> conn(new Connection(subLoops_[clientSock->Fd() % threadNum_].get(), move(clientSock))); /* 这里需要从事件循环 */
    /* 设置conns的回调函数，后面把这些回调函数移动到构造函数中，这样能防止忘记设置回调函数 */
    conn->SetCloseCallBackInConn(bind(&TcpServer::HandleCloseInTcpServer, this, placeholders::_1));
    conn->SetErrorCallBackInConn(bind(&TcpServer::HandleCloseInTcpServer, this, placeholders::_1));
    conn->SetReadCallBackInConn(bind(&TcpServer::HandleReadInTcpServer, this, placeholders::_1, placeholders::_2));
    conn->SetSendFinishCallBackInConn(bind(&TcpServer::HandleSendFinishInTcpServer, this, placeholders::_1));

    /* 将conn添加到conns容器中 */
    {
        lock_guard<mutex> lg(mutex_);
        conns_[conn->Fd()] = conn;
    }

    /* 这里还要把conn添加到subLoops_的conns_容器中，所以conn必须用shared_ptr */
    subLoops_[conn->Fd() % threadNum_]->AddConnToConns(conn);

    if (newConnCallBackInTcpServer_ != nullptr) {
        newConnCallBackInTcpServer_(conn);
    }
}

void TcpServer::OnEpollTimeout(EventLoop *loop)
{

}

void TcpServer::RunEventLoop()
{
    mainLoop_.RunLoop(); /* 从事件循环，从tcpserver构造的时候，就已经在run了，所以这里没有run */
}

void TcpServer::StopEventLoop()
{
    mainLoop_.StopLoop();

    for (uint32_t i = 0; i < threadNum_; i++) {
        subLoops_[i]->StopLoop();
    }
}

void TcpServer::SetNewConnCallBackInTcpServer(function<void(shared_ptr<Connection>)> fn)
{
    newConnCallBackInTcpServer_ = fn;
}

void TcpServer::HandleCloseInTcpServer(Connection *conn)
{
    {
        lock_guard<mutex> lg(mutex_);
        conns_.erase(conn->Fd());
    }
}
void TcpServer::HandleErrorInTcpServer(Connection *conn)
{
    {
        lock_guard<mutex> lg(mutex_);
        conns_.erase(conn->Fd());
    }
}
void TcpServer::HandleReadInTcpServer(Connection *conn, string &message)
{

}
void TcpServer::HandleSendFinishInTcpServer(Connection *conn)
{

}