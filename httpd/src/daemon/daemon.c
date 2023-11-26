//
// Created by Samuel DELIENS on 16/11/2023.
//
#define _POSIX_C_SOURCE 200809L

#include "daemon.h"

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

#include "server/server.h"

static FILE *pid_file;
static struct config *config;

int start_daemon(struct config *conf)
{
    config = conf;
    struct stat file_info;
    if (stat(config->pid_file, &file_info) == 0)
    {
        printf("Daemon already started\n");
        self_daemon_destroyed();
        exit(1);
    }

    pid_t pid = fork();

    if (pid < 0)
    {
        printf("Unable to fork\n");
        exit(2);
    }
    else if (pid != 0)
    {
        // Parent
        exit(0);
    }

    start_child();

    return 0;
}

void start_child(void)
{
    // Child
    printf("Daemon started\n");

    init_logger(config->log, true, config->log_file);

    LOGGER(NULL, "Daemon started with pid %d", getpid());
    LOGGER(NULL, "Daemon started with file %s", config->pid_file);

    pid_file = fopen(config->pid_file, "w");
    if (!pid_file)
    {
        printf("Unable to create pid file %s", config->pid_file);
        self_daemon_destroyed();
        exit(2);
    }
    fprintf(pid_file, "%d\n", getpid());
    fclose(pid_file);

    init_sig();

    start_server(config);

    close_logger();
}

void start_classic(struct config *conf)
{
    config = conf;
    init_sig();
    start_server(config);
    close_logger();
}

pid_t check_if_started(void)
{
    struct stat file_info;
    if (stat(config->pid_file, &file_info) != 0)
    {
        printf("Daemon not started\n");
        self_daemon_destroyed();
        exit(0);
    }

    pid_file = fopen(config->pid_file, "r");

    if (!pid_file)
    {
        printf("Unable to open pid file %s", config->pid_file);
        self_daemon_destroyed();
        exit(2);
    }

    char line[256];
    while (fgets(line, sizeof(line), pid_file) != NULL)
    {
        pid_t pid = atoi(line);
        if (pid > 0)
        {
            return pid;
        }
    }

    printf("Unable to find pid in file %s", config->pid_file);
    self_daemon_destroyed();
    exit(0);
}

void stop_daemon(struct config *conf)
{
    config = conf;
    struct stat file_info;

    pid_t pid = check_if_started();

    kill(pid, SIGINT);
    if (stat(config->pid_file, &file_info) == 0)
    {
        remove(config->pid_file);
    }
    self_daemon_destroyed();

    printf("Daemon stopped\n");

    exit(0);
}
void restart_daemon(struct config *conf)
{
    config = conf;
    struct stat file_info;

    pid_t pid = check_if_started();

    kill(pid, SIGINT);
    if (stat(config->pid_file, &file_info) == 0)
    {
        remove(config->pid_file);
    }
    printf("Daemon stopped\n");

    start_daemon(conf);
    exit(0);
}

void init_sig(void)
{
    struct sigaction action_term;
    action_term.sa_flags = 0;
    sigfillset(&action_term.sa_mask);
    action_term.sa_handler = handle_term;
    sigaction(SIGINT, &action_term, NULL);

    struct sigaction action_reload;
    action_reload.sa_flags = 0;
    sigfillset(&action_reload.sa_mask);
    sigdelset(&action_reload.sa_mask, SIGINT);
    action_reload.sa_handler = handle_term;
    sigaction(SIGHUP, &action_reload, NULL);
}
void handle_term(int sig)
{
    LOGGER(NULL, "Daemon stopped with signal %s", strsignal(sig));
    stop_server();
}
void handle_reload(int sig)
{
    LOGGER(NULL, "Received %s", strsignal(sig));
}

void self_daemon_destroyed(void)
{
    if (pid_file)
    {
        fclose(pid_file);
    }
    close_logger();
    if (config)
    {
        config_destroy(config);
    }
}
