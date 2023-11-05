// handover_server.cpp edited by Milad
#include "handover_server.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <vector>
#include <thread>
#include <mutex>
#include <stdexcept>

// Assume SRSRAN_SUCCESS and SRSRAN_ERROR are defined somewhere, e.g.:
#ifndef SRSRAN_SUCCESS
#define SRSRAN_SUCCESS 0
#endif

#ifndef SRSRAN_ERROR
#define SRSRAN_ERROR -1
#endif

class HandoverServer {
public:
    HandoverServer(int port);
    ~HandoverServer();

    int start();
    void stop();

private:
    void server_loop();
    static void handle_client(int client_socket);

    int port;
    bool is_running;
    int server_fd;
    std::thread server_thread;
    std::vector<std::thread> client_threads;
    std::mutex client_threads_mutex;
};

HandoverServer::HandoverServer(int port) : port(port), is_running(false), server_fd(-1) {}

HandoverServer::~HandoverServer() {
    stop();
}

int HandoverServer::start() {
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        std::cerr << "Failed to create socket: " << strerror(errno) << std::endl;
        return SRSRAN_ERROR;
    }

    struct sockaddr_in server_addr;
    std::memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "Failed to bind socket to port " << port << ": " << strerror(errno) << std::endl;
        close(server_fd);
        return SRSRAN_ERROR;
    }

    if (listen(server_fd, 10) < 0) {
        std::cerr << "Failed to listen on socket: " << strerror(errno) << std::endl;
        close(server_fd);
        return SRSRAN_ERROR;
    }

    is_running = true;
    try {
        server_thread = std::thread(&HandoverServer::server_loop, this);
    } catch (const std::exception& e) {
        std::cerr << "Failed to create server thread: " << e.what() << std::endl;
        close(server_fd);
        return SRSRAN_ERROR;
    }
    return SRSRAN_SUCCESS;
}

void HandoverServer::stop() {
    is_running = false;
    if (server_thread.joinable()) {
        server_thread.join();
    }

    {
        std::lock_guard<std::mutex> lock(client_threads_mutex);
        for (auto& thread : client_threads) {
            if (thread.joinable()) {
                thread.join();
            }
        }
    }

    if (server_fd != -1) {
        close(server_fd);
    }
}

void HandoverServer::server_loop() {
    while (is_running) {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        int client_socket = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);
        if (client_socket < 0) {
            if (!is_running) { // If the server is stopped, it's normal for accept to fail
                break;
            }
            std::cerr << "Failed to accept client connection: " << strerror(errno) << std::endl;
            continue;
        }

        try {
            std::lock_guard<std::mutex> lock(client_threads_mutex);
            client_threads.emplace_back(&HandoverServer::handle_client, client_socket);
        } catch (const std::exception& e) {
            std::cerr << "Failed to create client thread: " << e.what() << std::endl;
            close(client_socket);
        }
    }
}

void HandoverServer::handle_client(int client_socket) {
    // Handle communication with the connected client
    // ... (implementation remains the same)

    close(client_socket);
}
