//
// Created by Samuel DELIENS on 15/11/2023.
//

#ifndef SERVER_H
#define SERVER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "config/config.h"
#include "logger/logger.h"

/*
** @brief Start the server
**
** @param
**
** @return
*/
int start_server(struct config *config);
int stop_server(void);

int create_bind_socket(struct server_config *server_config);

int listen_client(struct server_config *server_config, int server_socket);

/*
** @brief Handle a client
**
** @param client_socket The client socket
**
** @return
*/
void handle_client(int client_socket);

#endif /* !SERVER_H */
