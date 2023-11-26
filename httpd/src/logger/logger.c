//
// Created by Samuel DELIENS on 16/11/2023.
//

#include "logger.h"

#include <stdarg.h>
#include <stdbool.h>
#include <time.h>

static bool log_enabled = false;
static char default_file[] = "HTTPd.log";
static FILE *log_file;

void init_logger(bool log, bool daemon, const char *logfile)
{
    if (log)
    {
        log_enabled = true;
        if (!logfile)
        {
            if (daemon)
            {
                logfile = default_file;
            }
            else
            {
                log_file = stdout;
            }
        }
        if (!log_file)
        {
            log_file = fopen(logfile, "w");
        }
    }
}

void LOGGER(struct string *server_name, const char *log_message, ...)
{
    if (!log_enabled)
        return;

    time_t log_time;
    struct tm *log_timeinfo;
    char buffer[80];

    time(&log_time);
    log_timeinfo = localtime(&log_time);
    strftime(buffer, sizeof(buffer), "%a, %d %m %Y %H:%M:%S GMT", log_timeinfo);
    va_list args;
    va_start(args, log_message);

    if (!log_file)
    {
        printf("Logger not initialized\n");
    }
    else
    {
        fprintf(log_file, "%s ", buffer);
        if (server_name)
        {
            fprintf(log_file, "[%s] ", server_name->data);
        }
        vfprintf(log_file, log_message, args);
        fprintf(log_file, "\n");
    }

    va_end(args);

    fflush(log_file);
}

void close_logger(void)
{
    if (log_file)
    {
        fclose(log_file);
    }
}
