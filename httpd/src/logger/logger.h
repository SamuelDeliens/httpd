//
// Created by Samuel DELIENS on 16/11/2023.
//

#ifndef LOGGER_H
#define LOGGER_H

#include <stdbool.h>
#include <stdio.h>
#include <time.h>

#include "utils/string/string.h"

void init_logger(bool log, bool daemon, const char *logfile);

void LOGGER(struct string *server_name, const char *log_message, ...);

void close_logger(void);

#endif /* !LOGGER_H */
