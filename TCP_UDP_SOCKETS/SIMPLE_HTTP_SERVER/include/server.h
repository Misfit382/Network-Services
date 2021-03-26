//
// Created by misfit on 13.03.2021.
//

#ifndef SERVER_SERVER_H
#define SERVER_SERVER_H

#define REQ_BUFF_SIZE 4096

#include <iostream>
#include <arpa/inet.h>

class server {
private:
    int port;
    int sock_fd{};
    std::string host;
    sockaddr_in server_addr{};
    void create_tcp_socket();
    void prepare_server();
    void bind_address();
    static bool check_if_http_req(const std::string& request);
    static void handle_client(const std::string& request, int conn_fd);
    static void *handle_connection(int conn_fd);
public:
    server(int _port, std::string _host);
    void start_server() const;
    static std::string get_filename_from_request(std::string request);
    static std::string get_extension_from_filename(std::string filename);
};


#endif //SERVER_SERVER_H
