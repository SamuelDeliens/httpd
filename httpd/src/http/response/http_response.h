//
// Created by Samuel DELIENS on 18/11/2023.
//

#ifndef HTTP_RESPONSE_H
#define HTTP_RESPONSE_H

#include <sys/types.h>

#include "config/config.h"
#include "utils/http_request/http_request.h"

int get_request(struct server_config *server_config, int client_socket,
                HttpRequest *http_request, char *response);
int head_request(struct server_config *server_config, int client_socket,
                 HttpRequest *http_request, char *response);
int error_request(struct server_config *server_conf, int client_socket,
                  char *response);

int header_response(char *response, size_t size);

int isDirectory(char *target);
int checkFile(char *response, char *target,
              struct server_config *server_config);

int send_response(int client_socket, char *response, char *target, off_t size);

#endif //HTTP_RESPONSE_H
