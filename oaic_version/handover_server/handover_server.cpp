// handover_server.cpp edited by Milad
#include "handover_server.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <string>
#include <thread>
#include <nlohmann/json.hpp>

// Constructor
HandoverServer::HandoverServer(int port) : port(port), is_running(false), server_fd(-1) {}

// Destructor
HandoverServer::~HandoverServer() {
    stop();
}

// Start the server
int HandoverServer::start() {
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        std::cerr << "Failed to create socket: " << strerror(errno) << std::endl;
        return SRSRAN_ERROR;
    }

    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        std::cerr << "Failed to set socket options: " << strerror(errno) << std::endl;
        close(server_fd);
        return SRSRAN_ERROR;
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

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
    server_thread = std::thread(&HandoverServer::server_loop, this);
    return SRSRAN_SUCCESS;
}

// Stop the server
void HandoverServer::stop() {
    is_running = false;
    if (server_thread.joinable()) {
        server_thread.join();
    }
    if (server_fd != -1) {
        close(server_fd);
    }
}

// Server loop to accept clients
void HandoverServer::server_loop() {
    while (is_running.load()) {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        int client_socket = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);
        if (client_socket < 0) {
            std::cerr << "Failed to accept client connection: " << strerror(errno) << std::endl;
            if (!is_running.load()) {
                break; // Server is stopping
            }
            continue; // Try to accept another connection
        }

        // Handle the client connection in a separate thread or function
        std::thread client_thread(&HandoverServer::handle_client, this, client_socket);
        client_thread.detach(); // Detach the thread to handle multiple clients
    }
}

void HandoverServer::handle_client(int client_socket) {
    const int buffer_size = 1024;
    char buffer[buffer_size] = {0};
    std::string data;

    std::cout << "Client connected: socket " << client_socket << std::endl;

    while (is_running.load()) {
        // Clear the buffer
        std::memset(buffer, 0, buffer_size);

        // Receive data from the client
        ssize_t bytes_received = recv(client_socket, buffer, buffer_size, 0);

        // Check for disconnection or errors
        if (bytes_received <= 0) {
            std::cerr << "Client disconnected or error occurred: socket " << client_socket << std::endl;
            break; // Exit the loop and close the socket
        }

        // Convert buffer to a std::string for easier handling
        data = std::string(buffer, bytes_received);

        // Attempt to parse the JSON data
        try {
            auto json_data = nlohmann::json::parse(data);

            // Extract the message ID, command, and parameters from JSON
            std::string message_id = json_data["message_id"];
            std::string command = json_data["command"];
            auto parameters = json_data["parameters"];

            std::cout << "Processing command: " << command << " with parameters " << parameters << std::endl;

            // Process the handover command
            // TODO: Add your logic here to handle the handover command and its parameters

            // Send a success response back to the client, including the message ID
            nlohmann::json response_json = {
                {"message_id", message_id},
                {"response", "Handover command processed."}
            };
            std::string response = response_json.dump() + "\n";
            send(client_socket, response.c_str(), response.size(), 0);
        }
        catch (const nlohmann::json::exception& e) {
            // Handle JSON parsing exceptions
            std::cerr << "JSON parsing error: " << e.what() << std::endl;

            // Send an error response back to the client
            nlohmann::json error_json = {
                {"error", "Invalid JSON message format."}
            };
            std::string response = error_json.dump() + "\n";
            send(client_socket, response.c_str(), response.size(), 0);
        }
    }

    // If we're here, the loop has ended, so we should clean up and close the socket
    std::cout << "Closing client socket: " << client_socket << std::endl;
    close(client_socket);
}
