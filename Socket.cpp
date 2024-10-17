#include "Socket.h"

int CreateNonBlocking()
{
    /* TODO: 研究阻塞与非阻塞socket的区别 */
    int listenFd = socket(AF_INET, SOCK_STREAM|SOCK_NONBLOCK, IPPROTO_TCP);
    if (listenFd < 0) {
        printf("%s:%s:%d listen socket create error:%d\n", __FILE__, __FUNCTION__, __LINE__, errno);
        exit(-1);
    }
    return listenFd;
}

Socket::Socket(int fd):fd_(fd)
{

}

Socket::~Socket()
{
    ::close(fd_);
}

void Socket::Bind(const InetAddress &servaddr)
{
    if (::bind(fd_, servaddr.Addr(), sizeof(sockaddr)) < 0)
    {
        perror("bind failed");
        close(fd_);
        return;
    }

    SetIpPort(servaddr.Ip(), servaddr.Port());
}

void Socket::Listen(int n)
{
    if (::listen(fd_, n) != 0) {
        perror("Listen failed");
        close(fd_);
        exit(-1);
    }
}

int Socket::Accept(InetAddress &clientAddr)
{
    sockaddr_in peerAddr;
    socklen_t len = sizeof(peerAddr); /* socklen_t就是uint32_t */
    int clientfd = accept4(fd_, (sockaddr *)&peerAddr, &len, SOCK_NONBLOCK);

    clientAddr.SetAddr(peerAddr);
    return clientfd;
}

void Socket::SetSockOpt(bool noDelaySw, bool reUseAddrSw, bool reUsePortSw, bool keepAliveSw)
{
    int optVal;
    size_t size = sizeof(optVal);

    optVal = noDelaySw ? 1 : 0;
    ::setsockopt(fd_, IPPROTO_TCP, TCP_NODELAY, &optVal, size);

    optVal = reUseAddrSw ? 1 : 0;
    ::setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR, &optVal, size);

    optVal = reUsePortSw ? 1 : 0;
    ::setsockopt(fd_, SOL_SOCKET, SO_REUSEPORT, &optVal, size);

    optVal = keepAliveSw ? 1 : 0;
    ::setsockopt(fd_, SOL_SOCKET, SO_KEEPALIVE, &optVal, size);
}

int Socket::Fd() const
{
    return fd_;
}

void Socket::SetIpPort(const string& ip, uint16_t port)
{
    ip_ = ip; /* TODO: const char *和string之间的关系 */
    port_ = port;
}