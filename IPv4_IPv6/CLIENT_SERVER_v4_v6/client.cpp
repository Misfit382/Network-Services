#include <netinet/in.h>
#include <cstring>
#include <net/if.h>
#include <unistd.h>
#include <netdb.h>
#include <iostream>

#define BUFF_SIZE 256

void validate_input(int argc, char **argv, int *port, std::string *host) {
    if (argc != 3) {
        std::cerr<<"[!] "<<"Wrong arguments:\n\tExecution: ./server3 <PORT>"<<std::endl;
        exit(EXIT_FAILURE);
    }
    *port = std::stoi(argv[2]);
    *host = (argv[1]);
}


int create_tcp_socket(const int domain) {
    return socket(domain, SOCK_STREAM, IPPROTO_TCP);
}

void setup_server(addrinfo *hints) {
    memset(&*hints, 0, sizeof(*hints));
    hints->ai_family   = AF_UNSPEC;
    hints->ai_socktype = SOCK_STREAM;
    hints->ai_protocol = IPPROTO_TCP;

}

int main(int argc, char *argv[]) {
    std::string host; int port;
    validate_input(argc, argv, &port, &host);
    int sock_fd;
    char buff[BUFF_SIZE];
    addrinfo hints{};
    addrinfo *result;

    setup_server(&hints);
    if ((getaddrinfo(host.c_str(), nullptr, &hints, &result)) != 0) {
        exit(EXIT_FAILURE);
    }

    sock_fd = create_tcp_socket(result->ai_family);
    if (result->ai_family == AF_INET) {
        sockaddr_in * tmp;
        tmp = (sockaddr_in *) result->ai_addr;
        tmp->sin_port = htons(port);
    }
    else {
        auto *tmp = (sockaddr_in6 *) result->ai_addr;
        tmp->sin6_port = htons(port);
    }



    if(connect(sock_fd, result->ai_addr, result->ai_addrlen)){
        std::cerr << "[!] " << "bind() error" << std::endl;
        close(sock_fd);
        exit(EXIT_FAILURE);
    }

    memset(buff, 0, sizeof(buff));
    recv(sock_fd, buff, BUFF_SIZE, 0);
    std::cout << buff;
    close(sock_fd);
    freeaddrinfo(result);
}