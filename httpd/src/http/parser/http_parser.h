#ifndef HTTP_PARSER_H
#define HTTP_PARSER_H

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "config/config.h"
#include "utils/http_request/http_request.h"

int parse_http_request(struct server_config *server_config,
                       HttpRequest *http_request, char *response,
                       const char *request);

int parse_startline(HttpRequest *http_request, const char *request);
int parse_headers(HttpRequest *http_request, const char *headers);

int parse_line(HttpRequest *http_request, char *request_headers, char *line);

int check_version(char *version);
int check_headers(Headers headers, Header *content_header);
int check_query(char *target);

int check_vhost(const char *host);

#endif /* !HTTP_PARSER_H */
