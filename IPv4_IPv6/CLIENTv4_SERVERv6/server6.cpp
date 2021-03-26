#include <iostream>
#include <arpa/inet.h>
#include <cstring>
#include <unistd.h>

void create_tcp_socket(int *sock_fd) {
    *sock_fd = socket(PF_INET6, SOCK_STREAM, IPPROTO_TCP); /*PF_INET - protokoły IPv4*/ /*SOCK_STREAM - gniazdo strumieniowe*/
    if (*sock_fd == -1) {
        std::cerr << "[!] " << "socket() error" << std::endl;
        exit(EXIT_FAILURE);
    }
}

void setup_server(const int *sock_fd, sockaddr_in6 *server_addr, int port) {
    memset(&*server_addr, 0, sizeof(*server_addr));
    server_addr->sin6_family = AF_INET6; /*AF_INET dla IPv6*/
    server_addr->sin6_addr = in6addr_any; /*adres nieokreślony*/
    server_addr->sin6_port = htons(port); /*convert values between host and network byte order*/

    int result = bind(*sock_fd, (struct sockaddr *) &*server_addr,sizeof(*server_addr)); /*bind() przypisuje gniazdu nazwę (dla protokołu IP jest to kombinacja adresu IP oraz 16 bitowego numeru portu).*/
    if (result == -1) {
        std::cerr << "[!] " << "bind() error" << std::endl;
        close(*sock_fd);
        exit(EXIT_FAILURE);
    }

    result = listen(*sock_fd, 1); /*przekształca gniazdo niepołączone w gniazdo bierne*/
    if (result == -1) {
        std::cerr << "[!] " << "listen() error" << std::endl;
        close(*sock_fd);
        exit(EXIT_FAILURE);
    }
}

int validate_input(int argc, char **argv) {
    if (argc != 2) {
        std::cerr<<"[!] "<<"Wrong arguments:\n\tExecution: ./server5 <PORT>"<<std::endl;
        exit(EXIT_FAILURE);
    }
    int port = std::stoi(argv[1]);
    return port;
}

int main(int argc, char **argv) {
    int sock_fd, port = validate_input(argc, argv);
    sockaddr_in6 srvr_addr{};
    create_tcp_socket(&sock_fd);
    setup_server(&sock_fd, &srvr_addr, port);

    while(true) {
        int conn_fd;
        sockaddr_in client_addr{};
        socklen_t client_addr_len;
        memset(&client_addr, 0, sizeof(sockaddr_in));
        client_addr_len = sizeof(sockaddr_in);

        conn_fd = accept(sock_fd, (sockaddr *) &client_addr, &client_addr_len);

        if((send(conn_fd, "Laboratorium PUS", strlen("Laboratorium PUS"), 0))== -1) {
            std::cerr << "[!] " << "send() error" << std::endl;
            close(sock_fd);
            exit(EXIT_FAILURE);
        }
    }
}
