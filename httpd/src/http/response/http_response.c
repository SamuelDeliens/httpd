//
// Created by Samuel DELIENS on 18/11/2023.
//

#include "http_response.h"

#include <fcntl.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/sendfile.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <unistd.h>

#include "logger/logger.h"

static struct server_config *server_config;

int get_request(struct server_config *server_conf, int client_socket,
                HttpRequest *http_request, char *response)
{
    server_config = server_conf;

    LOGGER(server_config->server_name, "GET request");

    char *target = calloc(1024, sizeof(char));
    strcpy(target, server_config->root_dir);
    strcat(target, "/");
    strcat(target, http_request->target);

    LOGGER(server_config->server_name, "target %s", target);

    int size = checkFile(response, target, server_config);
    if (size == -1)
    {
        size = 0;
        free(target);
        target = NULL;
    }

    header_response(response, size);
    LOGGER(server_config->server_name, "response %s", response);

    if (send_response(client_socket, response, target, size) == 2)
    {
        LOGGER(server_config->server_name, "Failed to send response");
        free(target);
        return 2;
    }

    free(target);
    return 0;
}
int head_request(struct server_config *server_conf, int client_socket,
                 HttpRequest *http_request, char *response)
{
    server_config = server_conf;
    LOGGER(server_config->server_name, "HEAD request");

    char *target = calloc(1024, sizeof(char));
    strcpy(target, server_config->root_dir);
    strcat(target, "/");
    strcat(target, http_request->target);

    LOGGER(server_config->server_name, "target %s", target);

    int size = checkFile(response, target, server_config);
    if (size == -1)
    {
        size = 0;
        target = NULL;
    }

    header_response(response, size);
    LOGGER(server_config->server_name, "response %s", response);

    if (send_response(client_socket, response, NULL, size) == 2)
    {
        LOGGER(server_config->server_name, "Failed to send response");
        free(target);
        return 2;
    }

    free(target);
    return 0;
}
int error_request(struct server_config *server_conf, int client_socket,
                  char *response)
{
    server_config = server_conf;
    LOGGER(server_config->server_name, "Error request");

    header_response(response, 0);
    send_response(client_socket, response, NULL, 0);

    return 0;
}

int header_response(char *response, size_t size)
{
    time_t now = time(0);
    struct tm tm = *gmtime(&now);
    char date[128];
    strftime(date, sizeof date, "%a, %d %b %Y %H:%M:%S %Z", &tm);

    size_t response_length = strlen(response) + 1024;
    size_t index = strlen(response);
    snprintf(response + index, response_length - index, "Date: %s\r\n", date);

    response_length = strlen(response) + 1024;
    index = strlen(response);
    snprintf(response + index, response_length - index,
             "Connection: close\r\n");

    response_length = strlen(response) + 1024;
    index = strlen(response);
    snprintf(response + index, response_length - index,
             "Content-Length: %zu\r\n\r\n", size);

    return 0;
}

int isDirectory(char *response)
{
    struct stat path_stat;
    stat(response, &path_stat);
    return S_ISDIR(path_stat.st_mode);
}
int checkFile(char *response, char *target, struct server_config *server_config)
{
    struct stat file_info;

    if (isDirectory(target) == 1)
    {
        if (server_config->default_file == NULL)
        {
            strcat(target, "/index.html");
        }
        else
        {
            strcat(target, "/");
            strcat(target, server_config->default_file);
        }
    }

    if (stat(target, &file_info) == 0)
    {
        if (!(file_info.st_mode & S_IROTH) || strstr(target, "..") != NULL)
        {
            strcpy(response, "HTTP/1.1 403 Forbidden\r\n");
            LOGGER(server_config->server_name, "Forbidden");
            return -1;
        }
        else
        {
            strcpy(response, "HTTP/1.1 200 OK\r\n");
            LOGGER(server_config->server_name, "File authorized %s", target);
        }
    }
    else
    {
        strcpy(response, "HTTP/1.1 404 Not Found\r\n");
        LOGGER(server_config->server_name, "Resources doesn't exist");
        return -1;
    }

    return file_info.st_size;
}

int send_response(int client_socket, char *response, char *target, off_t size)
{
    send(client_socket, response, strlen(response), 0);

    off_t offset = 0;
    ssize_t sent_bytes = 0;

    LOGGER(server_config->server_name, "Sending file...");

    int resource_d = open(target, O_RDONLY);

    if (size > 0)
    {
        while (offset < size)
        {
            sent_bytes =
                sendfile(client_socket, resource_d, &offset, size - offset);

            if (sent_bytes <= 0)
            {
                LOGGER(server_config->server_name, "File sent");
                close(resource_d);
                return 0;
            }
        }
    }
    return 0;
}
