#include "Connection.h"

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

}

Connection::~Connection()
{

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

                if (newConnCallBackInConn_ != nullptr) {
                    newConnCallBackInConn_(this, message);
                }
            }
            break;
        } else if (nread == 0) { /* 客户端连接已断开 */
            /* 处理客户端关闭事件 */
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
    clientChannel_->RemoveFromEpoll();
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
        clientChannel_->DisableWriting();
        if (sendFinishCallBackInConn_ != nullptr) {
            sendFinishCallBackInConn_(this);
        }
    }
}

void Connection::SetNewConnCallBackInConn(function<void(Connection *, string &)> fn)
{
    newConnCallBackInConn_ = fn;
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