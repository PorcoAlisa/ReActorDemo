#pragma once
#include "Socket.h"

using namespace std;

class Acceptor
{
private:
    Socket servSock_;
public:
    Acceptor(const string &ip, const uint16_t port);
    ~Acceptor();
    int Accept(InetAddress &clientAddr);
    void Read();
};