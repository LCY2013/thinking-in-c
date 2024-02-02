//
// Created by fufeng on 2024/2/2.
//
#ifndef THINKING_IN_C_STRUCT_H
#define THINKING_IN_C_STRUCT_H

#include <sys/socket.h>

// self-defined types.
#include <sys/socket.h>

// self-defined types.
typedef struct sockaddr_in sockaddr_in;
typedef struct sockaddr sockaddr;
typedef struct {
    int threadCount;
} serverSettings;
typedef struct {
    int serverFd;
    sockaddr* addr;
    socklen_t* addrLen;
} acceptParams;

#endif //THINKING_IN_C_STRUCT_H