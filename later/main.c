#include <raylib.h>

#include <enet/enet.h>
#include <string.h>
#include <stdio.h>
int main()
{
    enet_initialize();

    ENetHost *client;

    ENetAddress address;
    client = enet_host_create(NULL /* create a client host */,
                              1 /* only allow 1 outgoing connection */,
                              2 /* allow up 2 channels to be used, 0 and 1 */,
                              0 /* assume any amount of incoming bandwidth */,
                              0 /* assume any amount of outgoing bandwidth */);

    ENetEvent event;

    enet_address_set_host(&address, "127.0.0.1");
    address.port = 1234;

    while (enet_host_service(client, &event, 1000) > 0)
    {
        switch (event.type)
        {
        case ENET_EVENT_TYPE_CONNECT:
            printf("A new client connected from %x:%u.\n",
                   event.peer->address.host,
                   event.peer->address.port);
            static char clientInfo[] = "Client information";

            event.peer->data = clientInfo;

            break;

        case ENET_EVENT_TYPE_RECEIVE:
            printf("A packet of length %u containing %s was received from %s on channel %u.\n",
                   event.packet->dataLength,
                   event.packet->data,
                   event.peer->data,
                   event.channelID);

            enet_packet_destroy(event.packet);

            break;

        case ENET_EVENT_TYPE_DISCONNECT:
            printf("%s disconnected.\n", event.peer->data);

            event.peer->data = NULL;
        }
    }
    /* Create a reliable packet of size 7 containing "packet\0" */
    ENetPacket *packet = enet_packet_create("packet",
                                            strlen("packet") + 1,
                                            ENET_PACKET_FLAG_RELIABLE);

    /* Extend the packet so and append the string "foo", so it now */
    /* contains "packetfoo\0"                                      */
    enet_packet_resize(packet, strlen("packetfoo") + 1);
    strcpy(&packet->data[strlen("packet")], "foo");

    /* Send the packet to the peer over channel id 0. */
    /* One could also broadcast the packet by         */
    /* enet_host_broadcast (host, 0, packet);         */
    enet_peer_send(event.peer, 0, packet);

    /* One could just use enet_host_service() instead. */
    enet_host_flush(client);
    return 0;

    // enet_peer_disconnect() disconnect
}
