//
// Created by Samuel DELIENS on 15/11/2023.
//
#define _POSIX_C_SOURCE 200809L

#include "server.h"

#include <netdb.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "http/session.h"

#define MAX_CLIENTS 10

static bool run = true;

int start_server(struct config *config)
{
    LOGGER(NULL, "Starting the server");

    for (size_t i = 0; i < config->nb_servers; i++)
    {
        LOGGER(config->servers[i].server_name, "Starting server %s:%s",
               config->servers[i].ip, config->servers[i].port);

        int server_socket = create_bind_socket(&config->servers[i]);
        if (server_socket == -1)
        {
            return -1;
        }

        server_socket = listen_client(&config->servers[i], server_socket);
        if (server_socket == -1)
        {
            return -1;
        }

        LOGGER(config->servers[i].server_name, "Closing the server");
        return 0;
    }

    return 0;
}

int stop_server(void)
{
    run = false;
    return 0;
}

int create_bind_socket(struct server_config *server_config)
{
    struct addrinfo hints = { 0 };
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    struct addrinfo *result = NULL;

    if (getaddrinfo(server_config->ip, server_config->port, &hints, &result)
        != 0)
        return -1;

    int sock = -1;

    for (struct addrinfo *rp = result; rp; rp = rp->ai_next)
    {
        sock = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (sock == -1)
            continue;

        int optval = 1;
        setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

        if (bind(sock, rp->ai_addr, rp->ai_addrlen) != -1)
        {
            LOGGER(server_config->server_name, "Bind successful");
            break;
        }

        close(sock);
        sock = -1;
    }

    if (sock == -1)
        return -1;

    freeaddrinfo(result);
    return sock;
}

int listen_client(struct server_config *server_config, int sock)
{
    if (listen(sock, MAX_CLIENTS) != 0)
    {
        LOGGER(server_config->server_name, "Failed to listen");
        return -1;
    }

    while (run)
    {
        int client_socket = accept(sock, NULL, NULL);
        if (run == false)
        {
            break;
        }
        if (client_socket == -1)
        {
            LOGGER(server_config->server_name, "Failed to accept");
        }

        LOGGER(server_config->server_name, "Client connected");

        if (session_client(server_config, client_socket) != 0)
        {
            LOGGER(server_config->server_name, "Failed during client session");
        }

        close(client_socket);
        LOGGER(server_config->server_name, "Client disconnected");
    }

    LOGGER(server_config->server_name, "Closing the server");
    return 0;
}
