// main.cpp

#include "handover_server.h"
#include <iostream>

int main() {
    int port = 12345; // Replace with the port number you want the server to listen on
    HandoverServer server(port);

    if (server.start() != SRSRAN_SUCCESS) {
        std::cerr << "Failed to start server on port " << port << std::endl;
        return 1; // Return a non-zero value to indicate an error
    }

    std::cout << "Server started on port " << port << ". Press enter to stop the server..." << std::endl;
    std::cin.get(); // Wait for user input to stop the server

    server.stop();
    std::cout << "Server stopped." << std::endl;

    return 0; // Successful execution
}

