#include <iostream>
#include <arpa/inet.h>
#include <cstring>
#include <unistd.h>

#define BUFF_SIZE 256

/*Walidacja danych*/
void validate_input(int argc, char **argv, int *port, std::string *host) {
    if (argc != 3) {
        std::cerr<<"[!] "<<"Wrong arguments:\n\tExecution: ./server3 <PORT>"<<std::endl;
        exit(EXIT_FAILURE);
    }
    *port = std::stoi(argv[2]);
    *host = (argv[1]);
}

/*Stworzenie gniazda UDP*/
void create_tcp_socket(int *sockfd) {
    *sockfd = socket(PF_INET, SOCK_STREAM, 0);
    if (*sockfd == -1) {
        std::cerr<<"[!] "<<"socket() error"<<std::endl;
        exit(EXIT_FAILURE);
    }
}

/*ustawianie wartości dla klienta*/
void setup_client(struct sockaddr_in *remote_addr, socklen_t *addr_len, int port, char buff[BUFF_SIZE], const std::string& host) {
    memset(&*remote_addr, 0, sizeof(*remote_addr));
    remote_addr->sin_family = AF_INET;
    remote_addr->sin_port = htons(port);
    *addr_len = sizeof(*remote_addr);
    inet_pton(AF_INET, host.c_str(), &remote_addr->sin_addr);

}

int main(int argc, char **argv) {
    int port; std::string host;
    validate_input(argc, argv, &port, &host);
    std::cout<<host;

    int sockfd;
    char buff[BUFF_SIZE];
    sockaddr_in remote_addr{};
    socklen_t addr_len;

    create_tcp_socket(&sockfd);
    setup_client(&remote_addr, &addr_len, port, buff, host);
    int out = connect(sockfd, (sockaddr*)&remote_addr, addr_len);
    if(out) {
        std::cerr<<"[!] "<<"connect() error"<<std::endl;
        exit(EXIT_FAILURE);
    }
    memset(&buff, 0, sizeof(buff));
    recv(sockfd, buff, sizeof (buff), 0);
    std::cout<<buff;
    close(sockfd);
}