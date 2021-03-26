//
// Created by misfit on 14.03.2021.
//

#ifndef SERVER_REQUEST_BUILDER_H
#define SERVER_REQUEST_BUILDER_H
#include <iostream>

class request_builder {
private:
    std::string HTTP_OK = "HTTP/1.1 200 OK\r\n";
    std::string HTTP_ACCEPT_RANGES_BYTES = "accept-ranges: bytes\r\n";
    std::string HTTP_TEXT_CONTENT = "Content-type: text/";
    std::string HTTP_IMAGE_CONTENT = "Content-type: image/";
    std::string HTTP_TTF_CONTENT = "Content-type: font/ttf";

    std::string HTTP_CONTENT_LEN = "Content-Length: ";
    std::string HTTP_CONNECTION_CLOSE = "Connection: close\r\n";

public:
    std::string http_text(const std::string& extension) {
        if(extension != "js") {
            std::string header = HTTP_OK + HTTP_CONNECTION_CLOSE + HTTP_TEXT_CONTENT + extension + "\r\n\r\n";
            return header;
        }

        else {
            std::string header = HTTP_OK + HTTP_CONNECTION_CLOSE + HTTP_TEXT_CONTENT + "javascript" + "\r\n\r\n";
            return header;
        }
    }
    std::string http_image(const std::string& extension, size_t imagesize) {
        if(extension != "svg") {
            std::string header = HTTP_OK + HTTP_ACCEPT_RANGES_BYTES + HTTP_IMAGE_CONTENT + extension + "\n" + HTTP_CONTENT_LEN + std::to_string(imagesize) +"\r\n\r\n";
            return header;
        }
        else {
            std::string header = HTTP_OK + HTTP_ACCEPT_RANGES_BYTES + HTTP_IMAGE_CONTENT + extension + "+xml\n" + HTTP_CONTENT_LEN + std::to_string(imagesize) +"\r\n\r\n";
            return header;
        }
    }
    std::string http_ttf(const std::string& extension) {
            std::string header = HTTP_OK + HTTP_CONNECTION_CLOSE + HTTP_TTF_CONTENT + "\r\n\r\n";
        return header;
    }

};


#endif //SERVER_REQUEST_BUILDER_H
