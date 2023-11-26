//
// Created by Samuel DELIENS on 18/11/2023.
//

#include "http_request.h"

#include "logger/logger.h"

Header get_header(Headers headers, const char *header)
{
    Header result = { 0 };

    for (size_t i = 0; i < headers.size; i++)
    {
        if (strcmp(headers.headers[i].key, header) == 0)
        {
            result = headers.headers[i];
            break;
        }
    }

    return result;
}

void prepare_headers(Headers *headers)
{
    headers->headers = NULL;
}
void free_headers(Headers headers)
{
    for (size_t i = 0; i < headers.size; i++)
    {
        free(headers.headers[i].key);
        free(headers.headers[i].value);
    }
    free(headers.headers);
}

void prepare_http_request(HttpRequest *http_request)
{
    memset(http_request, 0, sizeof(HttpRequest));
    prepare_headers(&http_request->headers);
}
void free_http_request(HttpRequest *http_request)
{
    free(http_request->method);
    free(http_request->target);
    free(http_request->version);
    free_headers(http_request->headers);
    free(http_request);
}
