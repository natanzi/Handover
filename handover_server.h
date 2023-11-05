// handover_server.h // Edited by Milad
#ifndef HANDOVER_SERVER_H
#define HANDOVER_SERVER_H

#include <atomic>
#include <thread>
#include <vector>

class HandoverServer {
public:
    explicit HandoverServer(int port);
    ~HandoverServer();
    
    bool start();
    void stop();

private:
    int server_fd{-1};
    int port;
    std::atomic<bool> is_running{false};
    std::thread server_thread;
    std::vector<std::thread> client_threads;

    void server_loop();
    void handle_client(int client_socket);
};

#endif // HANDOVER_SERVER_H
