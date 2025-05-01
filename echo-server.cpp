#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <thread>
#include <set>

std::set<int> client_sockets;

void myerror(const char* msg) { fprintf(stderr, "%s %s %d\n", msg, strerror(errno), errno); }

void usage() {
    printf("syntax : echo-server <port> [-e[-b]]\n");
    printf("sample : echo-server 1234 -e -b\n");
}

struct Param {
    bool echo{false};
    bool broad{false};
    uint16_t port{0};

    bool parse(int argc, char* argv[]) {
        if (argc < 2) return false;
        port = atoi(argv[1]);
        for (int i = 2; i < argc; ++i) {
            if (strcmp(argv[i], "-e") == 0) echo = true;
            if (strcmp(argv[i], "-b") == 0) broad = true;
        }
        return port != 0;
    }
} param;

void recvThread(int sd) {
    static const int BUFSIZE = 65536;
    char buf[BUFSIZE];
    while (true) {
        ssize_t res = recv(sd, buf, BUFSIZE - 1, 0);
        if (res <= 0) break;
        buf[res] = '\0';
        printf("%s", buf);
        fflush(stdout);

        if (param.echo) {
            send(sd, buf, res, 0);
        }

        if (param.broad) {
            for (auto it = client_sockets.begin(); it != client_sockets.end();) {
                if (*it == sd) {
                    ++it;
                    continue;
                }

                ssize_t sent = ::send(*it, buf, res, 0);
                if (sent <= 0) {
                    myerror("broadcast send error");
                    it = client_sockets.erase(it);
                } else {
                    ++it;
                }
            }
        }

    }
    printf("disconnected\n");
    client_sockets.erase(sd);
    close(sd);
}

int main(int argc, char* argv[]) {
    if (!param.parse(argc, argv)) {
        usage();
        return -1;
    }

    int sd = socket(AF_INET, SOCK_STREAM, 0);
    if (sd == -1) {
        myerror("socket");
        return -1;
    }

    int optval = 1;
    setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(param.port);
    if (bind(sd, (sockaddr*)&addr, sizeof(addr)) == -1) {
        myerror("bind");
        return -1;
    }

    if (listen(sd, 5) == -1) {
        myerror("listen");
        return -1;
    }

    while (true) {
        sockaddr_in cli_addr;
        socklen_t len = sizeof(cli_addr);
        int newsd = accept(sd, (sockaddr*)&cli_addr, &len);
        if (newsd == -1) {
            myerror("accept");
            continue;
        }

        client_sockets.insert(newsd);
        std::thread(recvThread, newsd).detach();
    }

    close(sd);
}
