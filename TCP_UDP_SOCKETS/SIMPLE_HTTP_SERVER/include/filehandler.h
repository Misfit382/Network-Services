//
// Created by misfit on 13.03.2021.
//

#ifndef SERVER_FILEHANDLER_H
#define SERVER_FILEHANDLER_H

#include <iostream>
#include <vector>
#include <filesystem>
#include <string>
#include <fstream>
#include <algorithm>
class filehandler {
private:


public:
    std::vector<std::string> *imagefiles{};
    std::vector<std::string> *htmlfiles{};
    std::vector<std::string> *cssfiles{};
    std::string path;
    filehandler();
    [[nodiscard]] bool file_exists(const std::string& filename) const;
    size_t size_of_the_file(const std::string& filename);

    [[nodiscard]] std::string return_content_of_the_file(const std::string &filename) const;


    void image_buffer_to_send(const std::string& filename, int conn_fd) const;
};


#endif //SERVER_FILEHANDLER_H
