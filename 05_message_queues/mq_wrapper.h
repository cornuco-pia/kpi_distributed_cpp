#pragma once

#include <mqueue.h>
#include <string>
#include <stdexcept>
#include <vector>

// this is a simple wrapper to take advantage of RAII principle
class MessageQueue {
public:
    MessageQueue(const std::string & name, int oflag, mode_t mode = 0666, mq_attr* attr = nullptr);
    ~MessageQueue();
    void send(const std::string&msg, unsigned int prio = 0);
    std::string tryReceive();
    void closeOnly();
    static void unlinkQueue(const std::string & name);
private:
    mqd_t mq;
    std::string name_;
};