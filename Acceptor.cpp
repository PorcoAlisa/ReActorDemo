#include "Acceptor.h"

Acceptor::Acceptor(const string &ip, const uint16_t port, EventLoop *loop)
    :servSock_(CreateNonBlocking()), loop_(loop), acceptChannel_(loop_, servSock_.Fd())
{
    InetAddress servAddr(ip, port);
    servSock_.SetSockOpt(true, true, true, true);
    servSock_.Bind(servAddr);
    servSock_.Listen();
/* 直到Listen，已经完成了三次握手，并且服务器与客户端都进入established状态
服务端把这些已连接状态的socket放在一个已连接队列中，通过Accept可以把socket从已连接队列中取出来 */

/* 现在引入了acceptChannel的概念，那么初始化channel，就需要指定回调函数，除非发生事件后，你希望channel什么也不做 */
/* Acceptor的channel只处理新客户端连接的读事件，对其他事件不关心，所以指定读事件处理函数 */
    acceptChannel_.SetReadCallBack(bind(&Acceptor::AcceptNewConn, this));
    acceptChannel_.EnableReading();
}

Acceptor::~Acceptor()
{

}

// int Acceptor::Accept(InetAddress &clientAddr)
// {
//     return servSock_.Accept(clientAddr);
// }

void Acceptor::Read()
{
    char buf[1024];
    ssize_t nread = read(servSock_.Fd(), buf, sizeof(buf));

    printf("%s\n", buf);
}

/* 与socket中的accept区分开，将accept重命名为acceptnewconn */
void Acceptor::AcceptNewConn()
{
    InetAddress clientAddr; /* 调用Socket的Accept函数 */
/* 这里需要利用accept返回的fd创建一个socket，这是客户端的socket；
客户端的socket需要保存起来，不能用栈上的临时内存，需要申请内存，这里先用unique_ptr */
    unique_ptr<Socket> clientSock(new Socket(servSock_.Accept(clientAddr)));
    clientSock->SetIpPort(clientAddr.Ip(), clientAddr.Port());

/* Acceptor里面只能做到这一步，就是把客户端的socket创建好
接下来要干嘛，就属于业务需要关心的内容了，决不能放在acceptor里面
否则acceptor将和业务扯上关系而变得不再通用，业务随时可能变动，到时候acceptor也要随之变动，这不合理*/
    if (newConnCallBack_ != NULL) { /* 这里可以判断，如果newConnCallBack为空，打印日志，后续改进 */
        newConnCallBack_(move(clientSock)); /* unique_ptr用move转移所有权 */
    }
}

void Acceptor::SetNewConnCallBack(function<void(unique_ptr<Socket>)> fn)
{
    newConnCallBack_ = fn;
}