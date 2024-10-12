#include "Acceptor.h"

Acceptor::Acceptor(const string &ip, const uint16_t port):servSock_(CreateNonBlocking())
{
    InetAddress servAddr(ip, port);
    servSock_.SetSockOpt(true, true, true, true);
    servSock_.Bind(servAddr);
    servSock_.Listen();
}

Acceptor::~Acceptor()
{

}

int Acceptor::Accept(InetAddress &clientAddr)
{
    return servSock_.Accept(clientAddr);
}

void Acceptor::Read()
{
    char buf[1024];
    ssize_t nread = read(servSock_.Fd(), buf, sizeof(buf));

    printf("%s\n", buf);
}