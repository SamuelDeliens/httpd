//
// Created by Samuel DELIENS on 17/11/2023.
//

#define _POSIX_C_SOURCE 200809L

#include "http_parser.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "logger/logger.h"

static struct server_config *server_config;

int parse_http_request(struct server_config *server_conf,
                       HttpRequest *http_request, char *response,
                       const char *request)
{
    server_config = server_conf;

    LOGGER(server_config->server_name, "Parsing request %s", request);

    if (parse_startline(http_request, request) != 0)
    {
        strcpy(response, "HTTP/1.1 400 Bad Request\r\n");
        return 2;
    }

    if (check_version(http_request->version) != 0)
    {
        strcpy(response, "HTTP/1.1 505 HTTP Version Not Supported\r\n");
        return 2;
    }

    if (parse_headers(http_request, request) != 0)
    {
        strcpy(response, "HTTP/1.1 400 Bad Request\r\n");
        return 2;
    }

    Header *content_header = malloc(sizeof(Header));
    if (check_headers(http_request->headers, content_header) != 0)
    {
        strcpy(response, "HTTP/1.1 400 Bad Request\r\n");
        free(content_header);
        return 2;
    }

    LOGGER(server_config->server_name, "received %s on '%s' from %s",
           http_request->method, http_request->target, "127.0.0.1");
    free(content_header);
    return 0;
}

int parse_startline(HttpRequest *http_request, const char *request)
{
    size_t size = strlen(request);
    char *method = malloc(size);
    char *target = malloc(size);
    char *version = malloc(size);

    if (sscanf(request, "%s %s %s\r\n", method, target, version) != 3)
    {
        LOGGER(server_config->server_name, "Invalid request");
        return 2;
    }

    check_query(target);

    http_request->method = malloc(strlen(method) + 1);
    strcpy(http_request->method, method);
    http_request->target = malloc(strlen(target) + 1);
    strcpy(http_request->target, target);
    http_request->version = malloc(strlen(version) + 1);
    strcpy(http_request->version, version);

    free(method);
    free(target);
    free(version);

    return 0;
}
int parse_headers(HttpRequest *http_request, const char *request)
{
    LOGGER(server_config->server_name, "Parsing headers");

    // Header = Field-Name ":" Field-Value CRLF
    char *request_headers = calloc(strlen(request) + 1, sizeof(char));
    char *saveptr = NULL;
    strcpy(request_headers, request);

    http_request->headers.size = 0;
    http_request->headers.headers = NULL;

    char *line = strtok_r(request_headers, "\r\n", &saveptr);
    line = strtok_r(NULL, "\r\n", &saveptr);

    while (line != NULL)
    {
        if (parse_line(http_request, request_headers, line) == 2)
            return 2;

        http_request->headers.size++;

        line = strtok_r(NULL, "\r\n", &saveptr);
    }

    free(request_headers);
    return 0;
}

int parse_line(HttpRequest *http_request, char *request_headers, char *line)
{
    Header header = { NULL, NULL };

    header.key = strtok(line, " : ");
    header.value = strtok(NULL, " \r\n");

    if (header.key == NULL || header.value == NULL)
    {
        LOGGER(server_config->server_name, "Invalid header");
        free(request_headers);
        return 2;
    }

    size_t size = http_request->headers.size;
    Header *tmp =
        realloc(http_request->headers.headers, sizeof(Header) * (size + 1));
    if (tmp == NULL)
    {
        LOGGER(server_config->server_name, "Failed to realloc");
        free(request_headers);
        return 2;
    }
    http_request->headers.headers = tmp;
    http_request->headers.headers[size].key = strdup(header.key);
    http_request->headers.headers[size].value = strdup(header.value);
    if (http_request->headers.headers[size].key == NULL
        || http_request->headers.headers[size].value == NULL)
    {
        LOGGER(server_config->server_name, "Failed to strdup");
        free(request_headers);
        return 2;
    }
    return 0;
}

int check_version(char *version)
{
    LOGGER(server_config->server_name, "Checking version");

    if (strlen(version) != 8 || strncmp(version, "HTTP/", 5) != 0
        || version[6] != '.')
    {
        LOGGER(server_config->server_name, "Invalid version %s", version);
        return 2;
    }

    if (strcmp(version, "HTTP/1.1") == 0)
    {
        LOGGER(server_config->server_name, "HTTP/1.1");
        return 0;
    }
    else
    {
        LOGGER(server_config->server_name, "Unknown version %s", version);
        return 2;
    }
}
int check_query(char *target)
{
    LOGGER(server_config->server_name, "Checking query");

    if (strstr(target, "?") != NULL)
    {
        char *query = strchr(target, '?');
        if (query != NULL)
        {
            *query = '\0';
        }
    }
    return 0;
}
int check_headers(Headers headers, Header *content_header)
{
    LOGGER(server_config->server_name, "Checking headers");

    for (size_t i = 0; i < headers.size; i++)
    {
        Header header = headers.headers[i];
        LOGGER(server_config->server_name, "Header %s: %s", header.key,
               header.value);
    }

    Header host_header = get_header(headers, "Host");

    if (!host_header.key || !host_header.value
        || check_vhost(host_header.value) != 0)
    {
        LOGGER(server_config->server_name, "Invalid host");
        return 2;
    }

    Header header = get_header(headers, "Content-Length");
    content_header = &header;

    if (content_header->key && content_header->value)
    {
        LOGGER(server_config->server_name, "Content-Length: %s",
               content_header->value);
    }

    return 0;
}
int check_vhost(const char *host)
{
    // Server name; ip + port; ip
    LOGGER(server_config->server_name, "Checking vhost %s", host);

    if (strcmp(host, server_config->ip) == 0)
    {
        LOGGER(server_config->server_name, "Vhost %s", host);
        return 0;
    }
    else if (strcmp(host, server_config->server_name->data) == 0)
    {
        LOGGER(server_config->server_name, "Vhost %s", host);
        return 0;
    }
    else
    {
        char *ip_port =
            calloc(strlen(server_config->ip) + strlen(server_config->port) + 2,
                   sizeof(char));
        strcpy(ip_port, server_config->ip);
        strcat(ip_port, ":");
        strcat(ip_port, server_config->port);

        if (strcmp(host, ip_port) == 0)
        {
            LOGGER(server_config->server_name, "Vhost %s", host);
            free(ip_port);
            return 0;
        }

        free(ip_port);
        LOGGER(server_config->server_name, "Unknown vhost %s", host);
        return 2;
    }
}
