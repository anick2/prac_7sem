#include "sockets.h"
#include <sys/socket.h>


void PrintVec(std::vector<std::vector<int>> x, int z) {
    for (auto &i: x) {
        std::cout << z << " * ";
        for (auto &c : i) {
            std::cout << c << ' ';
        }
        std::cout << std::endl;
    }
}

int CreateSocket(const char *name) {
    int sockfd;
    struct sockaddr_un own_addr;

    memset(&own_addr, 0, sizeof(own_addr));
    own_addr.sun_family = AF_UNIX;

    if ((sockfd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
        printf("can't create socket\n");
        return 0;
    }
    strcpy(own_addr.sun_path, name);
    unlink(own_addr.sun_path);

    if (bind(sockfd, (struct sockaddr *) &own_addr, SUN_LEN (&own_addr)) < 0) {
        printf("can't bind socket!, i'm %s\n", name);
        return 0;
    }

    return sockfd;
}

Schedule ReadSolution(const int &client_socket_fd, const int &n, const int &m, const std::vector<int> &duration) {
    int length;
    if (read(client_socket_fd, &length, sizeof(length)) == 0)
        return NULL;
    else {
        std::vector<std::vector<int>> sol(length);

        for (int i = 0; i < length; i++) {
            int num;
            read(client_socket_fd, &num, sizeof(int));
            for (int j = 0; j < num; j++) {
                int k;
                read(client_socket_fd, &k, sizeof(int));
                sol[i].push_back(k);
            }
        }
        Schedule x(n, duration, sol);
        return x;
    }
}

void WriteSolution(int sockfd, std::vector<std::vector<int>> &sol) {

    int size = sol.size();
    write(sockfd, &size, sizeof(int));

    for (int i = 0; i < size; i++) {
        int size2 = sol[i].size();
        write(sockfd, &size2, sizeof(int));
        for (int j = 0; j < size2; j++) {
            write(sockfd, &sol[i][j], sizeof(int));
        }
    }
}

int GetClientSockFd(int sockfd){
    struct sockaddr_un client_name;
    socklen_t client_name_len;
    int client_socket_fd;
    client_socket_fd = accept(sockfd, (struct sockaddr *) &client_name, &client_name_len);
    if (client_socket_fd == -1) {
        std::cout << strerror(errno) << std::endl;
    }
    return client_socket_fd;
}

void Connect(int sockfd, const char* name){
    struct sockaddr_un party_addr;
    memset(&party_addr, 0, sizeof(party_addr));
    party_addr.sun_family = AF_UNIX;
    strcpy(party_addr.sun_path, name);

    while(connect(sockfd, (struct sockaddr *) &party_addr, SUN_LEN (&party_addr)) == -1);
}