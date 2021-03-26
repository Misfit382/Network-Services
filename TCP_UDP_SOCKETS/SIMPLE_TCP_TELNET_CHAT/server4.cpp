#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <cstring>

#define MAX_CLIENTS 5
#define BUFF_SIZE 256

/*KAŻDY MEMSET WYPEŁNIA WARTOŚCIA STRUKTURĘ ALBO BUFOR*/

/*ZWRACA LICZBE PODŁĄCZONYCH KLIENTÓW*/
int number_of_connected_clients(const int client_connections[MAX_CLIENTS]) {
    int active_connections = 0;
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (client_connections[i] > 0) {
            active_connections++;
        }
    }
    return active_connections;
}
/*WYSYŁA WIADOMOŚĆ DO KLIENTA*/
void send_message(int conn_fd, const char *message, const int *sock_fd) {
    int result = send(conn_fd, message, strlen(message), 0);
    if(result == -1) {
        std::cerr<<"[!] "<<"send() error"<<std::endl;
        close(*sock_fd);
        exit(EXIT_FAILURE);
    }
}
/*WYSYŁA WIADOMOŚĆ DO WSZYSTKICH KLIENTÓW*/
void send_to_all(int excludedfd, const char *message, const int *sock_fd, int client_connections[MAX_CLIENTS]) {
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (client_connections[i] > 0 && client_connections[i] != excludedfd) {
            send_message(client_connections[i], message, &*sock_fd);
        }
    }
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

/*Stworzenie gniazda TCP*/
void create_tcp_socket(int *sock_fd) {
    *sock_fd = socket(PF_INET, SOCK_STREAM, 0); /*PF_INET - protokoły IPv4*/ /*SOCK_STREAM - gniazdo strumieniowe*/
    if (*sock_fd == -1) {
        std::cerr << "[!] " << "socket() error" << std::endl;
        exit(EXIT_FAILURE);
    }
}

/*ustawianie wartości dla serwera*/
void setup_server(const int *sock_fd, sockaddr_in *server_addr, int port) {
    memset(&*server_addr, 0, sizeof(*server_addr));
    server_addr->sin_family = AF_INET; /*AF_INET dla IPv4*/
    server_addr->sin_addr.s_addr = htonl(INADDR_ANY); /*adres nieokreślony*/
    server_addr->sin_port = htons(port); /*convert values between host and network byte order*/

    int result = bind(*sock_fd, (struct sockaddr *) &*server_addr,sizeof(*server_addr)); /*bind() przypisuje gniazdu nazwę (dla protokołu IP jest to kombinacja adresu IP oraz 16 bitowego numeru portu).*/
    if (result == -1) {
        std::cerr << "[!] " << "bind() error" << std::endl;
        close(*sock_fd);
        exit(EXIT_FAILURE);
    }

    result = listen(*sock_fd, 2); /*przekształca gniazdo niepołączone w gniazdo bierne*/
    if (result == -1) {
        std::cerr << "[!] " << "listen() error" << std::endl;
        close(*sock_fd);
        exit(EXIT_FAILURE);
    }
}
int main(int argc, char **argv) {
    int port = validate_input(argc, argv);
    int sock_fd, client_connections[MAX_CLIENTS] = {0}, maxfd, result; /*kolejno: deskryptor nasłuchujący, lista deskryptorów,  maksymalny deksryptor dla wszystkich połączonych, deskryptory “gotowych” do przyjęcia*/

    fd_set readset; //  zbiór adresów podłączonych klientów
    sockaddr_in server_addr{}; //struktura przechowująca informacje adresowe dla protokołu IPv4

    create_tcp_socket(&sock_fd);
    setup_server(&sock_fd, &server_addr, port);

    while (true) {
        FD_ZERO(&readset);
        maxfd = sock_fd;
        FD_SET(sock_fd, &readset);// zerowanie zbioru
        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (client_connections[i] > 0) {
                FD_SET(client_connections[i], &readset); //dodanie do zbioru deskryptora podłączonego klienta
                if (client_connections[i] > maxfd) {
                    maxfd = client_connections[i];
                }
            }
        }
        result = select(maxfd + 1, &readset, nullptr, nullptr, nullptr); //oczekiwanie, aż jeden z deskryptorów będzie gotowy
        if (result == -1) {
            std::cerr << "[!] " << "select() error" << std::endl;
            close(sock_fd);
            exit(EXIT_FAILURE);
        }
        if (FD_ISSET(sock_fd, &readset)) {//  serwera czy klient
            struct sockaddr_in client_addr{};
            socklen_t addr_len = sizeof(client_addr);
            int new_clientfd = accept(sock_fd, (struct sockaddr *) &client_addr, &addr_len);
            if(new_clientfd == -1) {
                std::cerr<<"[!] "<<"accept() error"<<std::endl;
                close(sock_fd);
                exit(EXIT_FAILURE);
            }
            if (new_clientfd > 0) {
                char buf[BUFF_SIZE];
                memset(buf, 0, sizeof(buf));

                inet_ntop(AF_INET, (void *) &client_addr, buf, sizeof(buf));
                int client_port = ntohs(client_addr.sin_port);
                std::cout<<"[ok] "<<"Client " << buf<< " " << client_port << "has connected" <<std::endl;
                if (number_of_connected_clients(client_connections) < MAX_CLIENTS) {
                    for (int & client_connection : client_connections) {
                        if (client_connection <= 0) {
                            client_connection = new_clientfd;
                            break;
                        }
                    }
                    send_message(new_clientfd, "Connected succefully, ur id: %d\n", &sock_fd);
                    send_to_all(new_clientfd, "New client has connected id: %d\n", &sock_fd, client_connections);
                }
                else {
                    send_message(new_clientfd, "Connection refused\n", &sock_fd);
                    close(new_clientfd);
                }
            }
        }
        else {
            for (int cfd : client_connections) {
                if (cfd > 0 && FD_ISSET(cfd, &readset)) {
                    char buf[BUFF_SIZE];
                    memset(buf, 0, strlen(buf));
                    result = recv(cfd, buf, BUFF_SIZE, 0);
                    if(result == -1) {
                        std::cerr<<"[!] "<<"recv() error"<<std::endl;
                        close(sock_fd);
                        exit(EXIT_FAILURE);
                    }

                    std::string message_to_send;
                    message_to_send = "[" + std::to_string(cfd) + "] " + buf;

                    if (result == 0) {
                        for (int & client_connection : client_connections) {
                            if (client_connection == cfd) {
                                client_connection = 0;
                                send_to_all(cfd, "Guest has left",&sock_fd, client_connections);
                                std::cout<<"[!] "<<"Client " << cfd << "has left" << std::endl;
                                break;
                            }
                        }
                    }
                    else if (result > 0) {
                        send_to_all(cfd, message_to_send.c_str(), &sock_fd, client_connections);
                        std::cout<<message_to_send;
                    }
                }
            }
        }
    }
}