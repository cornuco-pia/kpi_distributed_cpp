#ifndef COMMON_H
#define COMMON_H

#include <string>
#include <cstring>

// made it static to prevent multiple definition linker errors
static const char* PIPE_NAME = "/tmp/lab07_fifo";

struct MessagePacket {
    int clientId;          // 1 or 2
    char name[50];
    double value;
    char description[100];
};

#endif