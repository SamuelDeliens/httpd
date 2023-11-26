//
// Created by Samuel DELIENS on 18/11/2023.
//

#ifndef HTTP_REQUEST_H
#define HTTP_REQUEST_H

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

// Headers dict
typedef struct
{
    char *key;
    char *value;
} Header;

typedef struct
{
    Header *headers;
    size_t size;
} Headers;

typedef struct
{
    char *method;
    char *target;
    char *version;
    Headers headers;
} HttpRequest;

Header get_header(Headers headers, const char *header);

void prepare_headers(Headers *headers);
void free_headers(Headers headers);

void prepare_http_request(HttpRequest *http_request);
void free_http_request(HttpRequest *http_request);

#endif /* !HTTP_REQUEST_H */
