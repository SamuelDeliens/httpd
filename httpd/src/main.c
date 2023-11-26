#define _POSIX_C_SOURCE 200809L

#include "config/config.h"
#include "daemon/daemon.h"
#include "logger/logger.h"
#include "server/server.h"

int dry_run(int argc, char *argv[], ssize_t i);
int daemon_run(int argc, char *argv[], ssize_t i);
int classic_run(int argc, char *argv[], ssize_t i);

// TODO ADD CHECK DAEMON TO DEFAULT LOG

int main(int argc, char *argv[])
{
    //./httpd [--dry-run] [-a (start | stop | reload | restart)] server.conf

    if (argc < 2)
    {
        fprintf(stderr,
                "Usage: ./httpd [--dry-run] [-a (start | stop | reload | "
                "restart)] server.conf\n");
        return 1;
    }

    ssize_t i = 1;

    if (strcmp(argv[i], "--dry-run") == 0)
    {
        if (dry_run(argc, argv, i))
        {
            return 1;
        };
    }

    else if (strcmp(argv[i], "-a") == 0)
    {
        if (daemon_run(argc, argv, i))
        {
            return 1;
        };
    }

    else
    {
        classic_run(argc, argv, i);
    }

    return 0;
}

int dry_run(int argc, char *argv[], ssize_t i)
{
    i++;

    if (i >= argc)
    {
        printf("Missing configuration file\n");
        return 1;
    }

    if (strcmp(argv[i], "-a") == 0)
    {
        i += 2;
    }

    if (i >= argc)
    {
        printf("Missing configuration file\n");
        return 1;
    }

    printf("Dry run\n");

    struct config *config = parse_configuration(argv[i]);

    printf("Configuration parsed: \n");

    config_destroy(config);

    printf("Configuration destroyed\n");
    return 0;
}

int daemon_run(int argc, char *argv[], ssize_t i)
{
    i++;
    if (i >= argc)
    {
        printf("Missing action\n");
        return 1;
    }

    const char *action = argv[i];
    i++;

    if (i >= argc)
    {
        printf("Missing configuration file\n");
        return 1;
    }

    struct config *config = parse_configuration(argv[i]);

    if (strcmp(action, "start") == 0)
    {
        start_daemon(config);
    }
    else if (strcmp(action, "stop") == 0)
    {
        stop_daemon(config);
    }
    else if (strcmp(action, "reload") == 0)
    {
        printf("Reload\n");
    }
    else if (strcmp(action, "restart") == 0)
    {
        restart_daemon(config);
    }
    else
    {
        printf("Unknown action\n");
        config_destroy(config);
        return 1;
    }

    config_destroy(config);
    return 0;
}

int classic_run(int argc, char *argv[], ssize_t i)
{
    if (i >= argc)
    {
        printf("Missing configuration file\n");
        return 1;
    }

    printf("Classic run\n");

    struct config *config = parse_configuration(argv[i]);
    init_logger(config->log, false, config->log_file);

    start_classic(config);

    config_destroy(config);

    return 0;
}
