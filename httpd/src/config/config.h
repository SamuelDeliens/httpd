#ifndef CONFIG_H
#define CONFIG_H

#include <stdbool.h>
#include <stddef.h>

#include "logger/logger.h"
#include "utils/string/string.h"

/*
** @brief Configuration structure
**
** @param pid_file Path to the pid file
** @param log_file Path to the log file
** @param log Enable or disable logging
** @param servers Array of vhosts
** @param nb_servers Number of vhosts
*/
struct config
{
    char *pid_file;
    char *log_file;
    bool log;

    struct server_config *servers;
    size_t nb_servers;
};

/*
** @brief Vhost configuration structure
**
** @param server_name Server name
** @param port Port to listen on
** @param ip IP address
** @param root_dir Root directory to serve
** @param default_file Default file to serve
*/
struct server_config
{
    struct string *server_name;
    char *port;
    char *ip;
    char *root_dir;
    char *default_file;
};

/*
** @brief Parse the configuration file and return a config struct
**        The config struct must be freed with config_destroy
**        If an error occurs, the function returns NULL
**
** @param path The path to the configuration file
**
** @return The config struct or NULL if an error occurs
*/
struct config *parse_configuration(const char *path);

/*
** @brief Free the config struct
**
** @param config The config struct to free
*/
void config_destroy(struct config *config);

#endif /* !CONFIG_H */
