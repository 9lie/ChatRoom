#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <iostream>
#include <thread>
#include <vector>
#include <string>
#include <map>

#define PORT 2434
#define IP "127.0.0.1"

int server_socket;
sockaddr_in servaddr;
socklen_t len;
std::vector<std::pair<int, std::string>> message;

struct Close {
    ~Close() {
        close(server_socket);
    }
} __CLOSE__;

void get_message(const int &sockfd) {
    char buf[256];
    while (true) {
        int len = read(sockfd, buf, sizeof(buf));
        if (len <= 0) {
            return;
        }
        buf[len] = 0;
        std::cout << sockfd << ' ' << buf << std::endl;
        message.emplace_back(std::make_pair(sockfd, std::string(buf)));
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

void send_message(const int &sockfd) {
    int p = 0;
    while (true) {
        while (p < message.size()) {
            if (message[p].first != sockfd) {
                int len = write(sockfd, message[p].second.c_str(), message[p].second.size());
                if (len < 0) {
                    return;
                }
            }
            p++;
        }
        std::this_thread::sleep_for(std::chrono:: milliseconds(100));
    }
}

int main() {
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    servaddr.sin_addr.s_addr = inet_addr(IP);
    if(bind(server_socket, (sockaddr*)&servaddr, sizeof(servaddr)) == -1 ) {
        perror("bind fail");
        exit(1);
    }
    if(listen(server_socket, 20) == -1) {
        perror("listen fail");
        exit(1);
    }
    printf("success\n");
    len = sizeof(servaddr);

    std::vector<int> sockfds;
    while (true) {
        int sockfd = accept(server_socket, (sockaddr*)&servaddr, &len);
        sockfds.push_back(sockfd);
        std::thread job1(get_message, sockfds.back());
        std::thread job2(send_message, sockfds.back());
        job1.detach();
        job2.detach();
        printf("%d connect", sockfd);
    }
}
