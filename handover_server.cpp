// handover_server.cpp edited by Milad

#include "handover_server.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <cstring>
#include <unistd.h>

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
    server_addr.sin_port = htons(this->port);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "Failed to bind socket to port " << this->port << ": " << strerror(errno) << std::endl;
        close(server_fd);
        return SRSRAN_ERROR;
    }

    if (listen(server_fd, 10) < 0) {
        std::cerr << "Failed to listen on socket: " << strerror(errno) << std::endl;
        close(server_fd);
        return SRSRAN_ERROR;
    }

    is_running = true;
    server_thread = std::thread(&HandoverServer::server_loop, this);
    return SRSRAN_SUCCESS;
}

void HandoverServer::stop() {
    is_running = false;
    if (server_thread.joinable()) {
        server_thread.join();
    }

    std::lock_guard<std::mutex> lock(client_threads_mutex);
    for (auto& thread : client_threads) {
        if (thread.joinable()) {
            thread.join();
        }
    }

    if (server_fd != -1) {
        close(server_fd);
    }
}

void HandoverServer::server_loop() {
    while (is_running.load()) {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        int client_socket = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);
        if (client_socket < 0) {
            if (!is_running.load()) {
                break;
            }
            std::cerr << "Failed to accept client connection: " << strerror(errno) << std::endl;
            continue;
        }

        client_threads.emplace_back(&HandoverServer::handle_client, this, client_socket);
    }
}

void HandoverServer::handle_client(int client_socket) {
    char buffer[1024] = {0};
    std::string data;

    ssize_t bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);
    if (bytes_received <= 0) {
        close(client_socket);
        return;
    }

    data = std::string(buffer, bytes_received);

    size_t newline_pos = data.find('\n');
    if (newline_pos != std::string::npos) {
        data.resize(newline_pos);
    }

    std::cout << "Command received, Going to apply handover...: " << data << std::endl;

    std::string response = "Handover acknowledged.\n";
    send(client_socket, response.c_str(), response.size(), 0);

    close(client_socket);
}
