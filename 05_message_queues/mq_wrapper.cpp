#include "mq_wrapper.h"
#include <cstring>
#include <unistd.h>
#include <cerrno>
#include <iostream>

MessageQueue::MessageQueue(const std::string & name, int oflag, mode_t mode, mq_attr * attr)
: name_(name)
{
    mq = mq_open(name.c_str(), oflag, mode, attr);
    if (mq == (mqd_t)-1) {
        throw std::runtime_error(std::string("mq_open") + std::strerror(errno));
    }
}

MessageQueue::~MessageQueue() {
    if (mq != (mqd_t)-1){
        mq_close(mq);
    }
}

void MessageQueue::closeOnly() {
    if (mq != (mqd_t)-1) {
        mq_close(mq);
        mq = (mqd_t)-1;
    }
}

void MessageQueue::send(const std::string & msg, unsigned int prio) {
    //struct mq_attr attr;
    //std::cout << "queue state:" << mq_getattr(mq, &attr) << std::endl; 
    if (mq_send(mq, msg.c_str(), msg.size()+1, prio) == -1) {
        throw std::runtime_error(std::string("mq_send") + std::strerror(errno));
    }
}

std::string MessageQueue::tryReceive() {
    if (mq == (mqd_t)-1) return {};
    struct mq_attr attr;
    if (mq_getattr(mq, &attr) == -1) return {};
    std::vector<char> buf(attr.mq_msgsize + 1);
    ssize_t bytes = mq_receive(mq, buf.data(), buf.size(), nullptr);
    if (bytes >= 0) {
        return std::string(buf.data());
    } else {
        //these specific errors occur when queue is full/empty for send/read
        //useless for now, does nothing different
        if (errno == EAGAIN || errno == EWOULDBLOCK) return {};
        return {};
    }
}

void MessageQueue::unlinkQueue(const std::string & name){
    mq_unlink(name.c_str());
}
