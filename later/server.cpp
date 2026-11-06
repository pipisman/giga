#include <enet/enet.h>
#include <cstdio>
#include <csignal>

namespace {
volatile std::sig_atomic_t running = 1;
void stop(int) { running = 0; }
}

int main()
{
    if (enet_initialize() != 0) {
        std::fprintf(stderr, "Failed to initialize ENet.\n");
        return 1;
    }

    ENetAddress address{};
    address.host = ENET_HOST_ANY;
    address.port = 1234;
    ENetHost* server = enet_host_create(&address, 32, 1, 0, 0);
    if (!server) {
        std::fprintf(stderr, "Could not start server on port 1234.\n");
        enet_deinitialize();
        return 1;
    }

    std::signal(SIGINT, stop);
    std::signal(SIGTERM, stop);
    std::puts("Server listening on port 1234. Press Ctrl+C to stop.");

    int result = 0;
    while (running) {
        ENetEvent event{};
        const int status = enet_host_service(server, &event, 100);
        if (status < 0) {
            std::fprintf(stderr, "Network error.\n");
            result = 1;
            break;
        }
        if (status == 0) continue; // Keep waiting when there are no events.

        switch (event.type) {
        case ENET_EVENT_TYPE_CONNECT:
            std::puts("Client connected.");
            break;
        case ENET_EVENT_TYPE_RECEIVE:
            std::printf("Received string (%zu bytes): ", event.packet->dataLength);
            // Use the packet length instead of assuming a terminating zero.
            std::fwrite(event.packet->data, 1, event.packet->dataLength, stdout);
            std::putchar('\n');
            enet_packet_destroy(event.packet);
            break;
        case ENET_EVENT_TYPE_DISCONNECT:
            std::puts("Client disconnected.");
            break;
        default:
            break;
        }
    }

    enet_host_destroy(server);
    enet_deinitialize();
    return result;
}
