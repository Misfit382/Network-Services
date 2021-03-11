#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include "libpalindrome.h"

#define BUFF_SIZE 256

/*KAŻDY MEMSET WYPEŁNIA WARTOŚCIA STRUKTURĘ ALBO BUFOR*/

/*Stworzenie gniazda UDP*/
void create_udp_socket(int *sockfd) {
    *sockfd = socket(PF_INET, SOCK_DGRAM, 0);
    if (*sockfd == -1) {
        std::cerr<<"[!] "<<"socket() error"<<std::endl;
        exit(EXIT_FAILURE);
    }
}
/*ustawianie wartości dla serwera*/
void setup_server(sockaddr_in *server_addr, socklen_t *server_addr_len, int port) {
    memset(&*server_addr, 0, sizeof(*server_addr));
    server_addr->sin_family =  AF_INET; /*AF_INET dla IPv4*/
    server_addr->sin_addr.s_addr = htonl(INADDR_ANY);/*adres nieokreślony*/
    server_addr->sin_port = htons(port); /*convert values between host and network byte order*/
    *server_addr_len = sizeof(*server_addr);
}

/*Walidacja danych*/
int validate_input(int argc, char **argv) {
    if (argc != 2) {
        std::cerr<<"[!] "<<"Wrong arguments:\n\tExecution: ./server3 <PORT>"<<std::endl;
        exit(EXIT_FAILURE);
    }
    int port = std::stoi(argv[1]);
    return port;
}

/*palindrom czy niepalindrom*/
bool proper_data(const char buff[BUFF_SIZE]) {
    bool is_number;
    for(int i=0;i<sizeof(*buff) && buff[i]!= '\n';i++) {
        if((buff[i]>='0' && buff[i]<='9')) {
            is_number = true;
        }
        else {
            is_number = false;
            break;
        }
        if(buff[i]==' ') {
            is_number = true;
            break;
        }
    }
    return is_number;
}
int main(int argc, char **argv) {

    int port = validate_input(argc, argv);
    int sockfd, retval = 0;
    char buff[BUFF_SIZE], addr_buff[BUFF_SIZE];
    sockaddr_in client_addr{}, server_addr{};
    socklen_t client_addr_len, server_addr_len;

    create_udp_socket(&sockfd);
    setup_server(&server_addr, &server_addr_len, port);

    if (bind(sockfd, (sockaddr*) &server_addr, server_addr_len) == -1) {
        std::cerr<<"[!] "<<"bind() error"<<std::endl;
        exit(EXIT_FAILURE);
    }

    std::cout<<"[ok] "<<"Server is listening for connections on: "<<inet_ntoa(server_addr.sin_addr)<<":"<<port<<std::endl;

    client_addr_len = sizeof(client_addr);
    while(true) {
        retval = recvfrom(sockfd, buff, sizeof(buff), 0, (sockaddr*)&client_addr, &client_addr_len);
        if (retval == -1) {
            std::cerr<<"[!] "<<"recvfrom() error"<<std::endl;
            exit(EXIT_FAILURE);
        }

        //adres z binarki w tekst
        inet_ntop(AF_INET, (sockaddr*)&client_addr.sin_addr,addr_buff,sizeof(addr_buff));
        std::cout<<"\n[~] "<<"Got a message from "<< addr_buff<< ": "<<buff<<std::endl;

        if(buff[0]=='\n') { //pusta wiadomość
            std::cout<<"[closing] "<<"Message is empty! "<<std::endl;
            close(sockfd);
            exit(EXIT_SUCCESS);
        }
        bool is_proper = proper_data(buff);

        char message[BUFF_SIZE];
        if(is_proper) { //sprawdzanie czy palindrom
            if(is_palindrome(buff,sizeof(buff))){
                std::cout<<" Sending that it is palindrome";
                strcpy(message, "Palindrome\n");
            }
            else {
                std::cout<<" Sending that it is not palindrome";
                strcpy(message, "Not Palindrome\n");
            }
        }
        else {
            strcpy(message, "Please give numbers only\n");
        }
        sendto(sockfd, message, sizeof(message), 0, (sockaddr*)&client_addr, client_addr_len); //słanie wiadomości
        memset(&message, 0, sizeof(message));
    }
}