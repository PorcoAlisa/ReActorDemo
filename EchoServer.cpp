#include "EchoServer.h"
#include <sys/syscall.h>

EchoServer::EchoServer(const string& ip, uint16_t port, uint32_t threadNum, uint32_t workThreadNum_, int timeout)
    :tcpServer_(ip, port, timeout, threadNum), workThreadPool_(workThreadNum_, "work")
{
    tcpServer_.SetNewConnCallBackInTcpServer(bind(&EchoServer::AcceptNewConnInEchoServer, this, placeholders::_1));

    tcpServer_.SetReadCallBackInTcpServer(bind(&EchoServer::HandleReadInEchoServer, this, placeholders::_1, placeholders::_2));
    tcpServer_.SetErrorCallBackInTcpServer(bind(&EchoServer::HandleErrorInEchoServer, this, placeholders::_1));
    tcpServer_.SetCloseCallBackInTcpServer(bind(&EchoServer::HandelCloseInEchoServer, this, placeholders::_1));
    tcpServer_.SetSendFinishCallBackInTcpServer(bind(&EchoServer::HandleSendFinishInEchoServer, this, placeholders::_1));
}

EchoServer::~EchoServer()
{
    printf("threadid = %d, EchoServer destructor\n", syscall(SYS_gettid));
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
    if (workThreadPool_.Size() == 0) { /* 说明没有工作线程，那就在事件循环线程中计算 */
        MessageProc(conn, message);
    } else { /* 有工作线程，交给工作线程去处理 */
        workThreadPool_.AddTask(bind(&EchoServer::MessageProc, this, conn, message));
    }
}

/* 这里想到一个问题，如果conn因为断开连接直接被shared_ptr给释放了，那这里会不会操作空指针呢？
所以我想这里应该把指针转换成shared_ptr再传入 */
void EchoServer::MessageProc(Connection *conn, string &message)
{
    printf("threadid = %d, MessageProc: %s\n", syscall(SYS_gettid), message.c_str());
    message = "reply: " + message;
    conn->SendInConn(message.data(), message.size());
}

void EchoServer::HandleErrorInEchoServer(Connection *conn)
{

}

void EchoServer::HandelCloseInEchoServer(Connection *conn)
{
    printf("threadid = %d, echoserver does not process close event temporarily.\n", syscall(SYS_gettid));
}

void EchoServer::HandleSendFinishInEchoServer(Connection *conn)
{

}
