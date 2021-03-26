#include <iostream>
#include <csignal>
#include "../include/server.h"
int main(int argc, char **argv) {
    auto *obj = new server(1233, "127.0.0.1");
    signal(SIGPIPE, SIG_IGN);
    obj->start_server();
    return 0;
}
