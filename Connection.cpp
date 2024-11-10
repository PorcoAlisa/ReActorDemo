#include "Connection.h"
#include <sys/syscall.h>

/* Channel必须在eventloop中运行，channel类中需要添加eventloop成员变量，标记channel所属的eventloop */
Connection::Connection(EventLoop *loop, unique_ptr<Socket> clientSock)
    :loop_(loop), clientSock_(move(clientSock)), clientChannel_(new Channel(loop_, clientSock_->Fd()))
{
    clientChannel_->SetReadCallBack(bind(&Connection::HandleReadEvent, this)); /* 把Connection的this指针传入，这样在channel类中就知道是哪个Connection发生了read事件 */
    clientChannel_->SetCloseCallBack(bind(&Connection::HandleCloseEvent, this));
    clientChannel_->SetErrorCallBack(bind(&Connection::HandleErrorEvent, this));
    clientChannel_->SetWriteCallBack(bind(&Connection::HandleWriteEvent, this));
    /* channel需要提供一个接口，对于客户端连接的channel，使用边缘触发模式 */
    clientChannel_->UseEt();
    clientChannel_->EnableReading();
    printf("threadid = %d, Connection construction: fd = %d\n", syscall(SYS_gettid), clientSock_->Fd());
}

Connection::~Connection()
{
    printf("threadid = %d, Connection destruction: fd = %d\n", syscall(SYS_gettid), clientSock_->Fd());
}

void Connection::HandleReadEvent()
{
    char buffer[1024];
    while (true) {
        bzero(&buffer, sizeof(buffer));
        ssize_t nread = read(Fd(), buffer, sizeof(buffer));
        if (nread > 0) {
            buf_.append(buffer, nread);
        } else if (nread == -1 && errno == EINTR) { /* 读取信号时被中断 */
            continue;
        } else if (nread == -1 && ((errno == EAGAIN) || (errno == EWOULDBLOCK))) { /* 全部数据已读取完毕 */
            string message;
            while (true) {
                if (buf_.size() == 0) {
                    break;
                }
                /* 假设采用4字节的报头 */
                uint32_t len;
                memcpy(&len, buf_.data(), 4);
                if (buf_.size() < len + 4) {
                    break;
                }
                message = buf_.substr(4, len);
                buf_.erase(0, len + 4);

                if (readCallBackInConn_ != nullptr) {
                    readCallBackInConn_(this, message);
                }
            }
            break;
        } else if (nread == 0) { /* 客户端连接已断开 */
            /* 处理客户端关闭事件 */
            printf("threadid = %d, HandleReadEvent nread = 0 shows client has closed. fd = %d\n", syscall(SYS_gettid), clientChannel_->Fd());
            HandleCloseEvent();
            break;
        }
    }
}

int Connection::Fd() const
{
    return clientSock_->Fd();
}

void Connection::HandleCloseEvent()
{
    /* 客户端关闭，要从事件循环中移除channel */
    printf("threadid = %d, client closed. fd = %d\n", syscall(SYS_gettid), clientChannel_->Fd());
    clientChannel_->RemoveFromEpoll();

    /* 从事件循环conns_中移除conn */
    loop_->RemoveConnFromConns(clientChannel_->Fd());

    if (closeCallBackInConn_ != nullptr) {
        closeCallBackInConn_(this);
    }
}

void Connection::HandleErrorEvent()
{
    clientChannel_->RemoveFromEpoll();
    if (errorCallBackInconn_ != nullptr) {
        errorCallBackInconn_(this);
    }
}

void Connection::HandleWriteEvent()
{
    int writen = send(Fd(), sendBuf_.data(), sendBuf_.size(), 0);
    if (writen > 0) {
        sendBuf_.erase(0, writen);
    }
    if (sendBuf_.size() == 0) {
        clientChannel_->DisableWriting(); /* 写完了，不需要了，就不再关注读事件了 */
        if (sendFinishCallBackInConn_ != nullptr) {
            sendFinishCallBackInConn_(this);
        }
    }
}

void Connection::SetReadCallBackInConn(function<void(Connection *, string &)> fn)
{
    readCallBackInConn_ = fn;
}

void Connection::SetCloseCallBackInConn(function<void(Connection *)> fn)
{
    closeCallBackInConn_ = fn;
}

void Connection::SetErrorCallBackInConn(function<void(Connection *)> fn)
{
    errorCallBackInconn_ = fn;
}

void Connection::SetSendFinishCallBackInConn(function<void(Connection *)> fn)
{
    sendFinishCallBackInConn_ = fn;
}

void Connection::SendInConn(const char *data, size_t size)
{
    /* 这里判断客户端是否已断开连接？ */

    /* 这里应该不能把sendBuf_清空，可能sendBuf_还有之前没发送完成的数据残留 */
    sendBuf_.append((char *)&size, 4);
    sendBuf_.append(data, size);
    clientChannel_->EnableWriting();
}