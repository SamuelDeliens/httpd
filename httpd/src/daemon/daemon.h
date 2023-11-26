#ifndef DAEMON_H
#define DAEMON_H

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

#include "config/config.h"
#include "logger/logger.h"
#include "server/server.h"

int start_daemon(struct config *config);
void start_classic(struct config *conf);

void start_child(void);

void stop_daemon(struct config *config);
void restart_daemon(struct config *config);

void init_sig(void);
void handle_term(int sig);
void handle_reload(int sig);

void self_daemon_destroyed(void);

#endif /* !DAEMON_H */
