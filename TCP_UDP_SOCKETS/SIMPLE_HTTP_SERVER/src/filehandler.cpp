//
// Created by misfit on 13.03.2021.
//
#include "../include/filehandler.h"
#include <arpa/inet.h>
filehandler::filehandler() {

    path = "www/";
}

size_t filehandler::size_of_the_file(const std::string& filename) {
    FILE *f = fopen((path + filename).c_str(), "r");
    size_t img_size = 0;
    if (f != nullptr) {
        fseek(f, 0, SEEK_END); // KONIEC PLIKU
        img_size = ftell(f);       // WIELKOŚĆ PLIKU
        fseek(f, 0, SEEK_SET); // POCZĄTEK PLIKU
    }
    fclose(f);
    return img_size;
}

std::string filehandler::return_content_of_the_file(const std::string& filename) const {

    std::ifstream file_input(path+filename);
    std::string file_content((std::istreambuf_iterator<char>(file_input)),std::istreambuf_iterator<char>());
    return file_content;
}
bool filehandler::file_exists(const std::string& filename) const {
    std::ifstream file_input(path+filename);
    if(file_input.good()) {
        file_input.close();
        return true;
    }
    else {
        file_input.close();
        return false;
    }
}

void filehandler::image_buffer_to_send(const std::string& filename, int conn_fd) const{
    FILE *f = fopen((path+filename).c_str(), "r");
    if(f != nullptr) {
        char img_buffer[10240];
        while (!feof(f)) {
            size_t read_size = fread(img_buffer, 1, 10240, f);
            send(conn_fd, img_buffer, read_size, 0);
        }
    }
    fclose(f);
}
