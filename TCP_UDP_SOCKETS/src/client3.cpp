#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <iostream>

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
void create_udp_socket(int *sockfd) {
    *sockfd = socket(PF_INET, SOCK_DGRAM, 0);
    if (*sockfd == -1) {
        std::cerr<<"[!] "<<"socket() error"<<std::endl;
        exit(EXIT_FAILURE);
    }
}

/*ustawianie wartości dla klienta*/
void setup_client(struct sockaddr_in *remote_addr, socklen_t *addr_len, int port, char buff[BUFF_SIZE]) {
    memset(&*remote_addr, 0, sizeof(*remote_addr));
    remote_addr->sin_family = AF_INET;
    remote_addr->sin_port = htons(port);
    *addr_len = sizeof(*remote_addr);
    memset(&buff, 0, sizeof(*buff));
}

int main(int argc, char **argv) {
    int port; std::string host;
    validate_input(argc, argv, &port, &host);

    int sockfd, retval = 0;
    char buff[BUFF_SIZE];
    sockaddr_in remote_addr{};
    socklen_t addr_len;

    create_udp_socket(&sockfd);
    setup_client(&remote_addr, &addr_len, port, buff);
    int out = connect(sockfd, (sockaddr*)&remote_addr, addr_len);
    if(out) {
        std::cerr<<"[!] "<<"connect() error"<<std::endl;
        exit(EXIT_FAILURE);
    }
    while(fgets(buff,sizeof(buff),stdin)) {
        send(sockfd,buff,sizeof(buff),0);
        if(buff[0]== '\n'){
            buff[0] = '\0';
            break;
        }
        retval = recv(sockfd,buff,sizeof(buff),0);

        if (retval == -1) {
            std::cerr<<"[!] "<<"recv() error"<<std::endl;
            exit(EXIT_FAILURE);
        }
        buff[retval]='\0';
        std::cout<<"[~] "<<"Server response: "<<buff;
        memset(&buff, 0, sizeof(buff));

    }
    close(sockfd);
    return 0;
}
