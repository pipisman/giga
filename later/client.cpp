#include <enet/enet.h>
#include <cstdio>
#include <cstring>

int main(int argc, char* argv[])
{
    const char* hostname = argc > 1 ? argv[1] : "127.0.0.1";
    const char* message = argc > 2 ? argv[2] : "Hello from the ENet client!";

    if (enet_initialize() != 0) {
        std::fprintf(stderr, "Failed to initialize ENet.\n");
        return 1;
    }

    ENetHost* client = enet_host_create(nullptr, 1, 1, 0, 0);
    if (!client) {
        std::fprintf(stderr, "Could not create client.\n");
        enet_deinitialize();
        return 1;
    }

    // Keep all exit paths responsible for releasing ENet resources.
    auto finish = [client](int result) {
        enet_host_destroy(client);
        enet_deinitialize();
        return result;
    };

    ENetAddress address{};
    address.port = 1234;
    if (enet_address_set_host(&address, hostname) != 0) {
        std::fprintf(stderr, "Could not resolve %s.\n", hostname);
        return finish(1);
    }

    ENetPeer* peer = enet_host_connect(client, &address, 1, 0);
    if (!peer) {
        std::fprintf(stderr, "Could not create connection.\n");
        return finish(1);
    }

    ENetEvent event{};
    if (enet_host_service(client, &event, 5000) <= 0 ||
        event.type != ENET_EVENT_TYPE_CONNECT) {
        std::fprintf(stderr, "Connection timed out. Is the server running?\n");
        enet_peer_reset(peer);
        return finish(1);
    }
    std::printf("Connected to %s:1234.\n", hostname);

    // ENet takes ownership of the packet after a successful send.
    ENetPacket* packet = enet_packet_create(message, std::strlen(message),
                                           ENET_PACKET_FLAG_RELIABLE);
    if (!packet) {
        std::fprintf(stderr, "Could not allocate packet.\n");
        return finish(1);
    }
    if (enet_peer_send(peer, 0, packet) != 0) {
        enet_packet_destroy(packet);
        std::fprintf(stderr, "Could not queue packet.\n");
        return finish(1);
    }

    // Disconnect only after queued reliable packets have been delivered.
    enet_peer_disconnect_later(peer, 0);
    const enet_uint32 started = enet_time_get();
    while (enet_time_get() - started < 5000) {
        const int status = enet_host_service(client, &event, 100);
        if (status < 0) break;
        if (status == 0) continue;
        if (event.type == ENET_EVENT_TYPE_RECEIVE)
            enet_packet_destroy(event.packet);
        if (event.type == ENET_EVENT_TYPE_DISCONNECT) {
            std::printf("Sent: %s\nDisconnected.\n", message);
            return finish(0);
        }
    }

    std::fprintf(stderr, "Timed out waiting for delivery/disconnect.\n");
    enet_peer_reset(peer);
    return finish(1);
}
