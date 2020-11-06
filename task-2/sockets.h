#pragma once

#include "solution.h"
#include <sys/socket.h>
#include <iostream>
#include <sys/un.h>
#include <unistd.h>


void PrintVec(std::vector<std::vector<int>> x, int z);

int CreateSocket(const char *name );

Schedule ReadSolution(const int &client_socket_fd, const int &n, const int &m, const std::vector<int> &duration);

void WriteSolution(int sockfd, std::vector<std::vector<int>> &sol);

int GetClientSockFd(int sockfd);

void Connect(int sockfd, const char* name);