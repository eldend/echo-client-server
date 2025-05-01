#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <iostream>
#include <thread>

void myerror(const char* msg) { fprintf(stderr, "%s %s %d\n", msg, strerror(errno), errno); }

void usage() {
    printf("syntax : echo-client <ip> <port>\n");
    printf("sample : echo-client 192.168.10.2 1234\n");
}

struct Param {
    char* ip{nullptr};
    char* port{nullptr};

    bool parse(int argc, char* argv[]) {
        if (argc < 3) return false;
        ip = argv[1];
        port = argv[2];
        return true;
    }
} param;

void recvThread(int sd) {
    static const int BUFSIZE = 65536;
    char buf[BUFSIZE];
    while (true) {
        ssize_t res = ::recv(sd, buf, BUFSIZE - 1, 0);
        if (res <= 0) {
            myerror("recv");
            break;
        }
        buf[res] = '\0';
        printf("%s", buf);
        fflush(stdout);
    }
    printf("disconnected\n");
    ::close(sd);
    exit(0);
}

int main(int argc, char* argv[]) {
    if (!param.parse(argc, argv)) {
        usage();
        return -1;
    }

    struct addrinfo hints = {}, *res;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    if (getaddrinfo(param.ip, param.port, &hints, &res) != 0) {
        myerror("getaddrinfo");
        return -1;
    }

    int sd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (sd == -1) {
        myerror("socket");
        return -1;
    }

    if (connect(sd, res->ai_addr, res->ai_addrlen) == -1) {
        myerror("connect");
        return -1;
    }

    std::thread t(recvThread, sd);
    t.detach();

    while (true) {
        std::string s;
        std::getline(std::cin, s);
        s += "\r\n";
        ssize_t res = ::send(sd, s.data(), s.size(), 0);
        if (res <= 0) {
            myerror("send");
            break;
        }
    }

    ::close(sd);
}
