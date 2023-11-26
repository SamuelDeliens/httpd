#define _POSIX_C_SOURCE 200809L

#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "logger/logger.h"

#define LINE_WIDTH 256

struct config *init_config(void);
void process_line(char *line, struct config *config);
struct config *check_config(struct config *config);

struct config *parse_configuration(const char *path)
{
    if (!path)
    {
        exit(2);
    }

    FILE *config_file = fopen(path, "r");

    if (!config_file)
    {
        exit(2);
    }

    struct config *config = init_config();
    if (!config)
    {
        fclose(config_file);
        exit(2);
    }

    char line[LINE_WIDTH];
    while (fgets(line, sizeof(line), config_file) != NULL)
    {
        process_line(line, config);
    }

    struct config *result = check_config(config);
    if (!result)
    {
        fclose(config_file);
        config_destroy(config);
        exit(2);
    }

    return config;
}

void config_destroy(struct config *config)
{
    if (config)
    {
        free(config->pid_file);
        free(config->log_file);

        for (size_t i = 0; i < config->nb_servers; ++i)
        {
            string_destroy(config->servers[i].server_name);
            free(config->servers[i].port);
            free(config->servers[i].ip);
            free(config->servers[i].root_dir);
            if (config->servers[i].default_file)
            {
                free(config->servers[i].default_file);
            }
        }
        free(config->servers);
        free(config);
    }
}

struct config *init_config(void)
{
    struct config *config = malloc(sizeof(struct config));
    if (!config)
    {
        return NULL;
    }

    config->servers = NULL;
    config->log = true;
    config->pid_file = NULL;
    config->log_file = NULL;
    config->nb_servers = 0;

    return config;
}
void process_line(char *line, struct config *config)
{
    if (strcmp(line, "\n") == 0 || strcmp(line, "\r\n") == 0)
    {
        return;
    }

    if (line[0] == '[')
    {
        if (strcmp(line, "[global]\n") == 0)
        {
            return;
        }
        else if (strcmp(line, "[[vhosts]]\n") == 0)
        {
            config->servers = realloc(config->servers,
                                      sizeof(struct server_config)
                                          * (config->nb_servers + 1));
            config->nb_servers++;
            memset(&config->servers[config->nb_servers - 1], 0,
                   sizeof(struct server_config));
            return;
        }
        else
        {
            return;
        }
    }

    char *key = strtok(line, " =\n");
    char *value = strtok(NULL, " =\n");

    if (!key || !value)
    {
        return;
    }

    if (strcmp(key, "log_file") == 0)
    {
        config->log_file = strdup(value);
    }
    else if (strcmp(key, "log") == 0)
    {
        config->log = strcmp(value, "true") == 0;
    }
    else if (strcmp(key, "pid_file") == 0)
    {
        config->pid_file = strdup(value);
    }
    else if (strcmp(key, "server_name") == 0)
    {
        config->servers[config->nb_servers - 1].server_name =
            string_create(value, strlen(value));
    }
    else if (strcmp(key, "port") == 0)
    {
        config->servers[config->nb_servers - 1].port = strdup(value);
    }
    else if (strcmp(key, "ip") == 0)
    {
        config->servers[config->nb_servers - 1].ip = strdup(value);
    }
    else if (strcmp(key, "root_dir") == 0)
    {
        config->servers[config->nb_servers - 1].root_dir = strdup(value);
    }
    else if (strcmp(key, "default_file") == 0)
    {
        config->servers[config->nb_servers - 1].default_file = strdup(value);
    }
    else
    {
        return;
    }
}
struct config *check_config(struct config *config)
{
    if (config->nb_servers == 0 || !config->pid_file)
    {
        return NULL;
    }
    for (size_t i = 0; i < config->nb_servers; ++i)
    {
        if (!config->servers[i].server_name || !config->servers[i].port
            || !config->servers[i].ip || !config->servers[i].root_dir)
        {
            return NULL;
        }
    }

    return config;
}
