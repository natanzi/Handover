// handover_server.h // Edited by Milad

#ifndef HANDOVER_SERVER_H
#define HANDOVER_SERVER_H

#include <atomic>
#include <thread>
#include <vector>
#include <mutex>

// Assume SRSRAN_SUCCESS and SRSRAN_ERROR are defined somewhere, e.g.:
#ifndef SRSRAN_SUCCESS
#define SRSRAN_SUCCESS 0
#endif

#ifndef SRSRAN_ERROR
#define SRSRAN_ERROR -1
#endif

class HandoverServer {
public:
    explicit HandoverServer(int port);
    ~HandoverServer();
    
    int start();
    void stop();

private:
    void server_loop();
    void handle_client(int client_socket);

    int server_fd{-1};
    int port;
    std::atomic<bool> is_running{false};
    std::thread server_thread;
    std::vector<std::thread> client_threads;
    std::mutex client_threads_mutex;
};

#endif // HANDOVER_SERVER_H

