#pragma once
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>
using namespace std;

class InetAddress
{
private:
    sockaddr_in addr_;
public:
    InetAddress();
    InetAddress(const string &ip, uint16_t port);
    ~InetAddress();
    const sockaddr * Addr() const;
    const char * Ip() const;
    const uint16_t Port() const;
    void SetAddr(sockaddr_in clientAddr);
};