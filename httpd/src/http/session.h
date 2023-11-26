//
// Created by Samuel DELIENS on 18/11/2023.
//

#ifndef SESSION_H
#define SESSION_H

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "config/config.h"
#include "logger/logger.h"
#include "parser/http_parser.h"
#include "response/http_response.h"
#include "session.h"
#include "utils/http_request/http_request.h"

int session_client(struct server_config *server_config, int client_socket);

int handle_request(struct server_config *server_config, int client_socket,
                   char *buffer);

int send_request(struct server_config *server_config, int client_socket,
                 HttpRequest *http_request, char *response);

#endif /* !SESSION_H */
