// main.cpp

#include "handover_server.h"
#include <iostream>

int main() {
    int port = 54321; // Replace with the desired port number for the server to listen on

    // Instantiate the server with the specified port
    HandoverServer server(port);

    // Start the server and check for successful launch
    if (server.start() != SRSRAN_SUCCESS) {
        std::cerr << "Failed to start server on port " << port << std::endl;
        return 1; // Return a non-zero value to indicate an error
    }

    // Inform the user that the server has started and provide a way to stop it
    std::cout << "Server started on port " << port << ". Press enter to stop the server..." << std::endl;
    std::cin.get(); // Wait for user input to stop the server

    // Stop the server upon user request
    server.stop();
    std::cout << "Server stopped." << std::endl;

    return 0; // Successful execution
}


