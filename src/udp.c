#include "include/udp.h"
#include "include/server.h"

void *udp_server()
{
// setup dependencies for win
#if defined(_WIN32)
    WSADATA d;
    if (WSAStartup(MAKEWORD(2, 2), &d))
    {
        fprintf(stderr, "Failed to initialize.\n");
        pthread_exit(NULL);
    }
#endif
    printf("Configuring remote address...\n");

    struct addrinfo hints;
    // this clears hints
    memset(&hints, 0, sizeof(hints));
    hints.ai_socktype = SOCK_DGRAM;

    // First paremter host name / ip
    // Second paremter service port or protical
    // Third addrinfo of hits in this case we input SOCK_STREAM to indecate that we want to use TCP
    // Lastly a pointer to the
    struct addrinfo *peer_address;
    if (getaddrinfo("127.0.0.1", "8200", &hints, &peer_address))
    {
        fprintf(stderr, "getaddrinfo() failed. (%d)\n", GETSOCKETERRNO());
        pthread_exit(NULL);
    }

    printf("Remote address is: ");

    char address_buffer[100];
    char service_buffer[100];

    // this does the revers of getaddrinfo it converts addrinfo back to strings in a protocl independedt way
    // debuging stage
    getnameinfo(
        peer_address->ai_addr,
        peer_address->ai_addrlen,
        address_buffer, sizeof(address_buffer),
        service_buffer, sizeof(service_buffer),
        NI_NUMERICHOST);

    printf("%s %s\n", address_buffer, service_buffer);

    printf("Creating socket...\n");
    // SOCKET is used becuse on windows its an unsinde int
    SOCKET socket_peer;

    // first is the set the protical ipv4 or ipv6
    // seckond is the typ udp or tcp
    // third is the protical used for the socket 0 takes one that will work
    socket_peer = socket(
        peer_address->ai_family,
        peer_address->ai_socktype,
        peer_address->ai_protocol);

    if (!ISVALIDSOCKET(socket_peer))
    {
        fprintf(stderr, "socket() failed. (%d)\n", GETSOCKETERRNO());
        pthread_exit(NULL);
    }

    printf("Connecting...\n");

    // first is the socket to connect to
    // seckond is the remote address -- the socket adress
    // third is the remote address lenght
    if (connect(socket_peer,
                peer_address->ai_addr,
                peer_address->ai_addrlen))
    {
        fprintf(stderr, "connect() failed. (%d)\n", GETSOCKETERRNO());
        pthread_exit(NULL);
    }
    freeaddrinfo(peer_address);

    printf("Connected.\n");

    while (1)
    {
        // a file descriptor
        fd_set reads;
        // zeros it
        FD_ZERO(&reads);

        // set to mointer socket
        FD_SET(socket_peer, &reads);

#if !defined(_WIN32)
        // 0 is the file discritor for stdin this is the same as FD_SET(fileno(stdin), &reads);
        // this moniters for terminal inputs
        FD_SET(0, &reads);
#endif

        // sets a time out on 100000 ms
        struct timeval timeout;
        timeout.tv_sec = 0;
        timeout.tv_usec = 100000;

        // select inducateds witch file descriptor thats ready for reading
        // first is the socket idk why + 1
        // the seckond, third and fourth is file discritor that it can track
        // lastly its a timeout

        if (select(socket_peer + 1, &reads, 0, 0, &timeout) < 0)
        {
            fprintf(stderr, "select() failed. (%d)\n", GETSOCKETERRNO());
            pthread_exit(NULL);
        }


        if (FD_ISSET(socket_peer, &reads))
        {
            udp_response_package_t response = {0};
            size_t size = sizeof(udp_response_package_t);
            char buffer[size];

            ssize_t bytes_received = recv(socket_peer, &buffer, sizeof(buffer), 0);

            if (bytes_received < 1)
            {
                printf("Connection closed by peer.\n");
                break;
            }

            memcpy(&response, buffer, size);

            int j = 0;
            for (int i = 0; i < MAX_CLIENTS; i++)
            {
                client_t client = response.clients[i];

                if (!strcmp(client.mac_address, connection.mac_address))
                {
                    memcpy(&opponents[j], &client, sizeof(client_t));
                    j++;
                }
            }
            
        }

        connection.udp_socket = socket_peer;
    }

    printf("Closing socket...\n");
    CLOSESOCKET(socket_peer);
//--------------------------------
// cleanup dependencies for win
#if defined(_WIN32)
    WSACleanup();
#endif

    printf("Finished.\n");

    pthread_exit(NULL);
}

void connect_udp_server()
{
    // Createing threads
    pthread_t tid;
    pthread_create(&tid, NULL, &udp_server, NULL);
}
