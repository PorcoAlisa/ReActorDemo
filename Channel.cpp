#include "Channel.h"

Channel::Channel(int fd):fd_(fd)
{

}

Channel::~Channel()
{
    
}

bool Channel::InEpoll()
{
    return inEpoll_;
}

uint32_t Channel::Events()
{
    return events_;
}

int Channel::Fd()
{
    return fd_;
}

void Channel::SetInEpoll(bool inEpoll)
{
    inEpoll_ = inEpoll;
}

void Channel::SetHappenedEvents(uint32_t happenedEvents)
{
    happenedEvents_ = happenedEvents;
}