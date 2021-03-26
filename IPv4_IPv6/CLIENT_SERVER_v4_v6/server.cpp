#include <netinet/in.h>
#include <cstring>
#include <iostream>
#include <unistd.h>

int validate_input(int argc, char **argv) {
    if (argc != 2) {
        std::cerr<<"[!] "<<"Wrong arguments:\n\tExecution: ./server5 <PORT>"<<std::endl;
        exit(EXIT_FAILURE);
    }
    int port = std::stoi(argv[1]);
    return port;
}

void setup_server(sockaddr_in6 *remote_addr, int port, socklen_t *remote_addr_len, const int *sock_fd) {
    memset(&*remote_addr, 0, sizeof (*remote_addr));
    remote_addr->sin6_family = AF_INET6;
    remote_addr->sin6_port = htons(port);
    remote_addr->sin6_addr   = in6addr_any;
    *remote_addr_len = sizeof(*remote_addr);

    if(bind(*sock_fd, (const struct sockaddr *) &*remote_addr, *remote_addr_len) == -1) {
        std::cerr<<"[!] "<<"bind err():\n"<<std::endl;
        close(*sock_fd);
        exit(EXIT_FAILURE);
    }
    if(listen(*sock_fd, 1)){
        std::cerr<<"[!] "<<"listen err():\n"<<std::endl;
        close(*sock_fd);

        exit(EXIT_FAILURE);
    }
}

void create_tcp_socket(int *sock_fd) {
    if((*sock_fd =socket(PF_INET6, SOCK_STREAM, IPPROTO_TCP)) == -1) {
        std::cerr<<"[!] "<<"socket err():\n"<<std::endl;
        close(*sock_fd);
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char **argv) {
    int sock_fd, port = validate_input(argc, argv);
    sockaddr_in6 srvr_addr{};
    socklen_t srvr_addr_len;

    create_tcp_socket(&sock_fd);
    setup_server(&srvr_addr,port,&srvr_addr_len, &sock_fd);


    while (true) {
        int connfd;
        struct sockaddr_in6 client_addr{};
        socklen_t client_addr_len;
        memset(&client_addr, 0, sizeof(sockaddr_in6));
        client_addr_len = sizeof(sockaddr_in6);

        connfd = accept(sock_fd, (sockaddr *) &client_addr, &client_addr_len);

        send(connfd, "Laboratorium PUS", strlen("Laboratorium PUS"), 0);
    }
}