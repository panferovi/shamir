#include <chrono>

#include "server/tcp_server.h"

int main()
{
    using namespace std::chrono_literals;

    shagit::TCPServer server;
    server.Start();

    while (!server.IsClosed()) {
        auto connections = server.GetConnections();
        std::this_thread::sleep_for(10min);
        if (server.GetConnections() == connections) {
            server.Close();
        }
    }

    server.Stop();
    return 0;
}
