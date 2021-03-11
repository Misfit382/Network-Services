#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <cstring>
#include <thread>
#include <filesystem>
#include <vector>
#include <algorithm>

#define BUFF_SIZE 4096
#define IMG_BUFF_SIZE 10240

/*KAŻDY MEMSET WYPEŁNIA WARTOŚCIA STRUKTURĘ ALBO BUFOR*/

/*Walidacja danych*/
int validate_input(int argc, char **argv) {
    if (argc != 2) {
        std::cerr<<"[!] "<<"Wrong arguments:\n\tExecution: ./server5 <PORT>"<<std::endl;
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

/*SZCZEGÓŁOWA OBSŁUGA*/
void handle_request(const std::string& request, int conn_fd) {
    std::string path = "../bin/img/"; /*Ścieżka do katalogu z plikami*/
    std::vector<std::string> tmpfilelist; /*WEKTOR Z WSZYSTKIMI PLIKAMI GRAFICZNYMI*/

    /* ITERUJEMY WSZYSTKO I WYBIERAMY TYLKO GRAFICZNE */
    for (const auto & entry : std::filesystem::directory_iterator(path)) {
        std::string temp = entry.path();
        temp.erase(std::remove(temp.begin(), temp.end(), '"'), temp.end());
        size_t pos = temp.find(path);
        if (pos != std::string::npos) {
            temp.erase(pos, path.length());
        }

        std::string ext = std::filesystem::path(entry.path()).extension();

        if(ext == ".jpg" || ext == ".png" || ext == ".jpeg" || ext == ".gif"){
            tmpfilelist.push_back(temp);
        }
    }

    bool found = false;
    int index, pos = 0;

    /*PATRZYMY CZY PLIK ISTNIEJE NA DYSKU*/
    for(const auto& filename : tmpfilelist) {
        while((index = request.find("GET /" + filename, pos)) != std::string::npos) {
            pos = index + 1;
            found = true;
            break;
        }
    }

    /*RZECZY DO REQUESTA */
    std::string httpok = "HTTP/1.1 200 OK\r\n";
    std::string http_accept_ranges_byte = "accept-ranges: bytes\r\n";
    std::string textcontent = "Content-type: text/html\r\n\r\n";
    std::string HTTP_CONTENT_LEN = "Content-Length: ";

    std::string connection_close = "Connection: close\r\n";
    std::string opentag = R"(<html><head><link rel="shortcut icon" href="data:image/x-icon;," type="image/x-icon"> </head><body><center>)";
    std::string closetag = "</html></body></center>";


    if(!found) { // JEŻELI CHCE BEZ PLIKU
        std::string http_request = httpok + connection_close + textcontent + opentag;

        for(const auto& a : tmpfilelist) {
            http_request += "<img src=\"" + a + "\" /><br>";
        }
        http_request += closetag;
        send(conn_fd, http_request.c_str(), strlen(http_request.c_str()), 0);
    }
    else { //JEŻELI CHCE PLIK
        std::string filename;
        pos = request.find(" HTTP/1.1");
        for(int i=0;i<pos;i++)
            filename+=request[i];

        filename.erase(0, 5); //WYCIĄGNIĘCIE NAZWY PLIKU

        // WYCIĄGNIĘCIE Z nazwy pliku rozszerzenia
        std::string extension;
        pos = filename.find('.');
        for(int i=pos+1; i<filename.size();i++)
            extension+=filename[i];

        // WCZYTANIE PLIKU DO PROGRAMU
        size_t img_size = 0;
        FILE *file = fopen((path+filename).c_str(),"r");
        if (file != nullptr) {
            fseek(file, 0, SEEK_END); // KONIEC PLIKU
            img_size = ftell(file);       // WIELKOŚĆ PLIKU
            fseek(file, 0, SEEK_SET); // POCZĄTEK PLIKU
        }
        else {
            std::cout<<"file doesnt exists"<<path+filename<<std::endl;
        }

        // BUDOWANIE REQUESTA nr 1 DLA PLIKU RESZTE POTEM SIE DOŚLE
        std::string image_header;
        image_header = httpok + http_accept_ranges_byte + image_header + extension+"\n" + HTTP_CONTENT_LEN  + std::to_string(img_size) +"\n\n";
        send(conn_fd, image_header.c_str(), image_header.size(), 0);
        if(file != nullptr) {
            char img_buffer[IMG_BUFF_SIZE];
            while (!feof(file)) {
                size_t read_size = fread(img_buffer, 1, IMG_BUFF_SIZE, file);
                send(conn_fd, img_buffer, read_size, 0); // TUTAJ JEST TO POTEM
            }
        }
    }

}

/*FUNCKJA NA WĄTKU*/
void *handle_connection(int conn_fd) {
    char *buff  = static_cast<char *>(malloc(BUFF_SIZE)); //REQUEST
    memset(buff, 0, BUFF_SIZE);
    std::string temp;
    if (recv(conn_fd, buff, BUFF_SIZE, 0) < 0) { //WCZYTANIE REQUESTA
        close(conn_fd);
    }
    temp.assign(buff, BUFF_SIZE);
    handle_request(temp, conn_fd); //OBSŁUGA REQUSTA


    close(conn_fd);
    std::this_thread::yield();
    return nullptr;
}

int main(int argc, char **argv) {
    int port = validate_input(argc, argv);
    int sock_fd, result; /*sock_fd deskryptor*/ /*deskryptor dla clienta*/

    sockaddr_in server_addr{};/*przechowująca informacje adresowe dla protokołu IPv4*/

    create_tcp_socket(&sock_fd);
    setup_server(&sock_fd, &server_addr, port);

    while(true) {
        sockaddr_in client_addr{}; /*przechowująca informacje adresowe dla protokołu IPv4*/
        socklen_t addr_len = sizeof(client_addr); /*mówi o rozmiarze struktury */
        if ((result = accept(sock_fd, (sockaddr *) &client_addr, &addr_len)) > 0) {
            std::thread conn_thread(handle_connection, result);
            conn_thread.join();
        }
    }

}
