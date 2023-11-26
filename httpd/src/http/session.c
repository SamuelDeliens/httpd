//
// Created by Samuel DELIENS on 18/11/2023.
//

#include "session.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

#define BUFFER_SIZE 4096

int session_client(struct server_config *server_config, int client_socket)
{
    LOGGER(server_config->server_name, "Client Connected");

    ssize_t read_bytes;
    ssize_t total_read_bytes = 0;
    char total_buffer[BUFFER_SIZE] = { 0 };
    char buffer[BUFFER_SIZE] = { 0 };

    while ((read_bytes = recv(client_socket, buffer, BUFFER_SIZE, 0)) > 0)
    {
        total_read_bytes += read_bytes;
        strcat(total_buffer, buffer);
        LOGGER(server_config->server_name, "Received %ld bytes", read_bytes);
        LOGGER(server_config->server_name, "Received data from client %s",
               total_buffer);

        if (read_bytes == -1)
            return 2;
        else if (strstr(total_buffer, "\r\n\r\n") == NULL)
        {
            memset(buffer, 0, BUFFER_SIZE);
            continue;
        }

        if (handle_request(server_config, client_socket, total_buffer) == 2)
            return 2;

        memset(total_buffer, 0, BUFFER_SIZE);
        return 0;
    }

    return 0;
}

int handle_request(struct server_config *server_config, int client_socket,
                   char *buffer)
{
    LOGGER(server_config->server_name, "buffer %s", buffer);

    HttpRequest *http_request = malloc(sizeof(HttpRequest));
    prepare_http_request(http_request);

    char *response = calloc(1024, 1024 * sizeof(char));

    if (parse_http_request(server_config, http_request, response, buffer)
        == 2)
    {
        LOGGER(server_config->server_name, "received Bad Request from %s",
               "127.0.0.1");
        LOGGER(server_config->server_name, "responding with 400 to %s",
               "127.0.0.1");
        error_request(server_config, client_socket, response);
        free_http_request(http_request);
        free(response);

        return 2;
    }

    // TODO check if get correct content length

    if (send_request(server_config, client_socket, http_request, response) == 2)
    {
        free_http_request(http_request);
        free(response);
        return 2;
    }

    free_http_request(http_request);
    free(response);
    return 0;
}

int send_request(struct server_config *server_config, int client_socket,
                 HttpRequest *http_request, char *response)
{
    if (strcmp(http_request->method, "GET") == 0)
    {
        LOGGER(server_config->server_name,
               "responding with 200 to %s for %s on '%s'", "127.0.0.1",
               http_request->method, http_request->target);
        if (get_request(server_config, client_socket, http_request, response)
            == 2)
        {
            LOGGER(server_config->server_name, "Failed to get request");
            return 2;
        }
    }
    else if (strcmp(http_request->method, "HEAD") == 0)
    {
        LOGGER(server_config->server_name,
               "responding with 200 to %s for %s on '%s'", "127.0.0.1",
               http_request->method, http_request->target);
        if (head_request(server_config, client_socket, http_request, response)
            == 2)
        {
            LOGGER(server_config->server_name, "Failed to head request");
            return 2;
        }
    }
    else
    {
        LOGGER(server_config->server_name,
               "responding with 405 to %s for UNKNOWN on '%s'", "127.0.0.1",
               http_request->target);
        strcpy(response, "HTTP/1.1 405 Method Not Allowed\r\n");
        error_request(server_config, client_socket, response);
        return 2;
    }
    return 0;
}

void free_session(HttpRequest *http_request, char *response)
{
    free_http_request(http_request);
    free(response);
}
