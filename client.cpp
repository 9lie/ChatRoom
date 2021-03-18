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

#define PORT 2434
#define IP "127.0.0.1"

int client_socket;
sockaddr_in cliaddr;
socklen_t len;

struct Close {
    ~Close() {
        close(client_socket);
    }
} __CLOSE__;

void get_message() {
    char buf[256];
    while (true) {
        int len = read(client_socket, buf, sizeof(buf));
        if (len <= 0) {
            return;
        }
        buf[len] = 0;
        printf("%s\n", buf);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

int main() {
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    cliaddr.sin_family = AF_INET;
    cliaddr.sin_port = htons(PORT);
    cliaddr.sin_addr.s_addr = inet_addr(IP);
    if (connect(client_socket, (sockaddr*)&cliaddr, sizeof(cliaddr)) < 0) {
        perror("connect fail");
        exit(1);
    }
    printf("%d connected\n", client_socket);
    std::thread job1(get_message);
    job1.detach();
    
    std::string buf;
    while (true) {
        std::cin >> buf;
        int len = write(client_socket, buf.c_str(), buf.size());
        if (len <= 0) {
        	exit(0);
        }
    }
}
