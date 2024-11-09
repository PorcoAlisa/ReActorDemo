#include "Epoll.h"
#include <sys/syscall.h>

Epoll::Epoll()
{
    epollFd_ = epoll_create(1);
    if (epollFd_ == -1) {
        printf("epoll_create() failed(%d).\n", errno);
        exit(-1);
    }
    printf("threadid = %d, Epoll Constructor: epollFd_ = %d\n", syscall(SYS_gettid), epollFd_);
}

Epoll::~Epoll()
{
    close(epollFd_);
    printf("threadid = %d, Epoll destructor: epollFd_(%d) closed.\n", syscall(SYS_gettid), epollFd_);
}

void Epoll::UpdateChannel(Channel *ch)
{
    epoll_event ev;
    ev.data.ptr = ch;
    ev.events = ch->Events();

    int ret = 0;

    if (ch->InEpoll()) {
        ret = epoll_ctl(epollFd_, EPOLL_CTL_MOD, ch->Fd(), &ev);
        if (ret != 0) {
            perror("epoll_ctl() MOD failed.\n");
            exit(-1);
        }
    } else {
        ret = epoll_ctl(epollFd_, EPOLL_CTL_ADD, ch->Fd(), &ev);
        if (ret != 0) {
            perror("epoll_ctl() ADD failed.\n");
            exit(-1);
        }
        /* 当channel被添加到epoll模型时，第一时间设置inEpoll_为true */
        ch->SetInEpoll(true);
    }
}

void Epoll::RemoveChannel(Channel *ch)
{
    int ret = 0;
    if (ch->InEpoll()) {
        ret = epoll_ctl(epollFd_, EPOLL_CTL_DEL, ch->Fd(), 0);
        if (ret != 0) {
            perror("epoll_ctl() DEL failed.\n");
            exit(-1);
        }
        ch->SetInEpoll(false);
    }
}

vector<Channel *> Epoll::EpollWait(int timeout)
{
    vector<Channel *> channels;
    bzero(events_, sizeof(events_));
    int fdNums = epoll_wait(epollFd_, events_, MAX_EVENTS, timeout);

    /* 失败 */
    if (fdNums < 0) {
        perror("epoll_wait() failed.\n");
        exit(-1); /* 这里的处理有待琢磨 */
    }

    /* 超时 */
    if (fdNums == 0) {
        return channels;
    }

    for (int i = 0; i < fdNums; i++) {
        Channel *ch = (Channel *)events_[i].data.ptr;
        ch->SetHappenedEvents(events_[i].events);
        channels.push_back(ch);
    }

    return channels;
}