#pragma once
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>
using namespace std;

/*
struct sockaddr_in {  
  unsigned short sin_family;	// 协议族，与socket()函数的第一个参数相同，填AF_INET。
  unsigned short sin_port;		// 16位端口号，大端序。用htons(整数的端口)转换。
  struct in_addr sin_addr;		// IP地址的结构体。192.168.101.138
  unsigned char sin_zero[8];	// 未使用，为了保持与struct sockaddr一样的长度而添加。
};
struct in_addr {				// IP地址的结构体。
  unsigned int s_addr;		// 32位的IP地址，大端序。
};
*/

class InetAddress
{
private:
    sockaddr_in addr_; /* 协议族是默认的AF_INET，主要是ip和port */
public:
    InetAddress(); /* 为什么需要一个没有参数的构造函数，因为accept的时候，不知道ip和port，正是需要accept来写入ip和port */
    InetAddress(const string &ip, uint16_t port);
    ~InetAddress();
    const sockaddr * Addr() const;
    const char * Ip() const;
    const uint16_t Port() const;
    void SetAddr(sockaddr_in clientAddr);
};