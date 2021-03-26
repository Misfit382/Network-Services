#include <iostream>
#include <utility>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <cstring>
#include <unistd.h>
#include <thread>
#include <algorithm>


#include "../include/server.h"
#include "../include/filehandler.h"
#include "../include/request_builder.h"

void server::bind_address() {
    int result = bind(sock_fd, (sockaddr *) &server_addr,sizeof(server_addr));
    if (result == -1) {
        std::cerr << "[!] " << "bind() error" << std::endl;
        close(sock_fd);
        exit(EXIT_FAILURE);
    }
}

void server::prepare_server() {
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(port);
}

bool server::check_if_http_req(const std::string& request) {
    int pos = 0, index;
    while((index = request.find("GET /", pos)) != std::string::npos) {
        pos = index + 1;
        return true;
    }
    return false;
}

void server::handle_client(const std::string& request, int conn_fd) {
    std::string filename = get_filename_from_request(request);
    std::cout<<request;
    if(!request.find("GET / HTTP/1.1"))
        filename = "index.html";
    std::string extension = get_extension_from_filename(filename);




    auto *file = new filehandler();
    auto *header = new request_builder();
    if(file->file_exists(filename)) {
        if(extension == "jpg" || extension == "jpeg" || extension == "gif" || extension == "png" || extension == "ico" || extension == "svg") {
            std::string HTTP_REQUEST = header->http_image(extension, file->size_of_the_file(filename));
            send(conn_fd, HTTP_REQUEST.c_str(), HTTP_REQUEST.size(), 0);
            file->image_buffer_to_send(filename, conn_fd);

        }
        else if(extension == "ttf") {
            std::string HTTP_REQUEST = header->http_ttf(extension);
            send(conn_fd, HTTP_REQUEST.c_str(), HTTP_REQUEST.size(), 0);
            send(conn_fd, file->return_content_of_the_file(filename).c_str(), file->return_content_of_the_file(filename).size(), 0);

        }
        else if(extension == "html" || extension == "css" || extension == "js") {
            std::string HTTP_REQUEST = header->http_text(extension);
            send(conn_fd, HTTP_REQUEST.c_str(), HTTP_REQUEST.size(), 0);
            send(conn_fd, file->return_content_of_the_file(filename).c_str(), file->return_content_of_the_file(filename).size(), 0);
        }
    }
    else send(conn_fd, "file does not exists\n", strlen("file does not exists\n"), 0);




}

std::string server::get_filename_from_request(std::string request) {
    std::string filename;
    int pos = request.find(" HTTP/1.1");
    for(int i=0;i<pos;i++)
        filename+=request[i];
    filename.erase(0, 5);
    return filename;
}

std::string server::get_extension_from_filename(std::string filename) {
    std::string extension;
    int pos;
    pos = filename.find_last_of('.');
    for(int i=pos+1; i<filename.size();i++)
        extension+=filename[i];
    return extension;
}

void *server::handle_connection(int conn_fd) {
    std::string req_buff;
    char *buff = static_cast<char *>(malloc(REQ_BUFF_SIZE));;
    if (recv(conn_fd, buff, REQ_BUFF_SIZE, 0) < 0) {
        close(conn_fd);
    }
    req_buff.assign(buff, REQ_BUFF_SIZE);
    if(check_if_http_req(req_buff))
        handle_client(req_buff, conn_fd);
    else
        send(conn_fd, "try via http\n", strlen("try via http\n"), 0);
    close(conn_fd);
    std::this_thread::yield();
    return nullptr;
}

server::server(int _port, std::string _host) {
    port = _port;
    host = std::move(_host);
    create_tcp_socket();
    prepare_server();
    bind_address();
}

void server::start_server() const {
    int result = listen(sock_fd, 2);
    if (result == -1) {
        std::cerr << "[!] " << "listen() error" << std::endl;
        close(sock_fd);
        exit(EXIT_FAILURE);
    }

    while(true) {
        sockaddr_in client_addr{};
        socklen_t addr_len = sizeof(client_addr);
        if ((result = accept(sock_fd, (sockaddr *) &client_addr, &addr_len)) > 0) {
            std::thread conn_thread(handle_connection, result);
            conn_thread.join();
        }
    }
}

void server::create_tcp_socket() {
    sock_fd = socket(PF_INET, SOCK_STREAM, 0);
    if (sock_fd == -1) {
        std::cerr << "[!] " << "socket() error" << std::endl;
        exit(EXIT_FAILURE);
    }
}
