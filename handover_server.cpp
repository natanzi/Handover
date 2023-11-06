// handover_server.cpp edited by Milad

#include "handover_server.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <string>

// Constructor
HandoverServer::HandoverServer(int port) : port(port), is_running(false), server_fd(-1) {}

// Destructor
HandoverServer::~HandoverServer() {
    stop();
}

// Start the server
int HandoverServer::start() {
    // ... existing start code ...

    is_running = true;
    server_thread = std::thread(&HandoverServer::server_loop, this);
    return SRSRAN_SUCCESS;
}

// Stop the server
void HandoverServer::stop() {
    // ... existing stop code ...
}

// Server loop to accept clients
void HandoverServer::server_loop() {
    // ... existing server_loop code ...
}

// Handle client connections
void HandoverServer::handle_client(int client_socket) {
    const int buffer_size = 1024;
    char buffer[buffer_size] = {0};
    std::string data;
    std::string keep_alive_msg = "KEEP_ALIVE\n";
    std::string ack_msg = "ACK\n";

    while (is_running.load()) {
        // Clear the buffer
        std::memset(buffer, 0, buffer_size);

        // Receive data from the client
        ssize_t bytes_received = recv(client_socket, buffer, buffer_size, 0);

        // Check for disconnection or errors
        if (bytes_received <= 0) {
            std::cerr << "Client disconnected or error occurred.\n";
            break; // Exit the loop and close the socket
        }

        // Convert buffer to a std::string for easier handling
        data = std::string(buffer, bytes_received);

        // Check if this is a keep-alive message
        if (data == keep_alive_msg) {
            // Send an acknowledgment back to the client
            send(client_socket, ack_msg.c_str(), ack_msg.size(), 0);
        } else {
            // Handle other messages
            std::cout << "Received message: " << data << std::endl;

            // Assume any other message is a handover command and process it
            std::cout << "Command received, Going to apply handover...: " << data << std::endl;

            // Send a success response back to the client
            std::string response = "Handover acknowledged.\n";
            send(client_socket, response.c_str(), response.size(), 0);
        }
    }

    // If we're here, the loop has ended, so we should clean up and close the socket
    close(client_socket);
}

