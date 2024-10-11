#include "InetAddress.h"

InetAddress::InetAddress()
{

}

/* 监听的fd使用的构造函数 */
InetAddress::InetAddress(const string &ip, uint16_t port)
{
    addr_.sin_family = AF_INET;
    addr_.sin_addr.s_addr = inet_addr(ip.c_str());
    addr_.sin_port = htons(port);
}

InetAddress::~InetAddress()
{

}

const sockaddr * InetAddress::Addr() const
{
    return (sockaddr *)&addr_;
}

const char * InetAddress::Ip() const
{
    return inet_ntoa(addr_.sin_addr);
}

const uint16_t InetAddress::Port() const
{
    /* 这里存储的端口是网络序，需要转换成主机序 */
    return ntohs(addr_.sin_port);
}

void InetAddress::SetAddr(sockaddr_in clientAddr)
{
    addr_ = clientAddr;
}