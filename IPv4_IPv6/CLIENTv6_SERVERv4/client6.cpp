#include <cstring>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <net/if.h>
#include <iostream>

#define BUFF_SIZE 256

void validate_input(int argc, char **argv, int *port, std::string *host) {
    if (argc != 4) {
        std::cerr<<"[!] "<<"Wrong arguments:\n"<<std::endl;
        exit(EXIT_FAILURE);
    }
    *port = std::stoi(argv[2]);
    *host = (argv[1]);
}

void setup_server(sockaddr_in6 *remote_addr, int port, char *iface, const std::string& host) {
    memset(&*remote_addr, 0, sizeof (*remote_addr));
    remote_addr->sin6_family = AF_INET6;
    remote_addr->sin6_port = htons(port);
        remote_addr->sin6_scope_id = if_nametoindex(iface);
    inet_pton(AF_INET6, host.c_str(), &remote_addr->sin6_addr);
}

void *get_in_addr(struct sockaddr *sa) {
    return &(((struct sockaddr_in6*) sa)->sin6_addr);
}


int main(int argc, char** argv)
{
    int sock_fd = 0, port;
    std::string host;
    validate_input(argc, argv, &port, &host);
    sockaddr_storage s_sock{};
    socklen_t sockaddr_len = sizeof (sockaddr_storage);
    sockaddr_in6 remote_addr{};

    char buff[BUFF_SIZE];
    char serv[NI_MAXSERV];
    setup_server(&remote_addr, port, argv[3], host);
    if((sock_fd = socket(AF_INET6, SOCK_STREAM, 0))==-1) {
        std::cerr<<"[!] "<<"socket() error"<<std::endl;
        exit(EXIT_FAILURE);
    }
    if (connect(sock_fd, (struct sockaddr*) &remote_addr, sizeof (struct sockaddr_in6)) == -1) {
        std::cerr<<"[!] "<<"connect() error"<<std::endl;
        close(sock_fd);
        exit(EXIT_FAILURE);
    }
    if(getsockname(sock_fd, (struct sockaddr *) &s_sock, &sockaddr_len) == -1) {
        std::cerr<<"[!] "<<"getsocketname() error"<<std::endl;
        close(sock_fd);
        exit(EXIT_FAILURE);
    }
    inet_ntop(s_sock.ss_family, get_in_addr((sockaddr *) &s_sock), (char *)host.c_str(), sizeof(host.c_str()));
    getnameinfo((struct sockaddr *) &s_sock, sockaddr_len, (char *)host.c_str(), NI_MAXHOST, serv, NI_MAXSERV, NI_NUMERICHOST | NI_NUMERICSERV);

    memset(buff, 0, sizeof (buff));
    recv(sock_fd, buff, sizeof (buff), 0);
    std::cout<<buff;
    close(sock_fd);
    exit(EXIT_SUCCESS);
}