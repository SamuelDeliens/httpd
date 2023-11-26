#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <unistd.h>

#define MAX_EVENTS 5
#define READ_SIZE 10

int setup_epoll(int fifo_fd);
void handle_event(int epoll_fd, int fifo_fd);
int check_event(int fifo_fd);

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Usage: %s <fifo_name>\n", argv[0]);
        return 1;
    }

    int fifo_fd = open(argv[1], O_RDONLY | O_NONBLOCK);
    if (fifo_fd == -1)
        return 1;

    int epoll_fd = setup_epoll(fifo_fd);
    if (epoll_fd == -1)
    {
        close(fifo_fd);
        return 1;
    }

    handle_event(epoll_fd, fifo_fd);

    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fifo_fd, NULL);
    close(epoll_fd);
    close(fifo_fd);
    return 0;
}

int setup_epoll(int fifo_fd)
{
    int epoll_fd = epoll_create1(0);
    if (epoll_fd == -1)
    {
        return -1;
    }

    struct epoll_event event;
    event.events = EPOLLIN;
    event.data.fd = fifo_fd;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fifo_fd, &event) == -1)
    {
        close(epoll_fd);
        return -1;
    }
    return epoll_fd;
}
void handle_event(int epoll_fd, int fifo_fd)
{
    struct epoll_event events[MAX_EVENTS];
    while (1)
    {
        int n = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
        for (int i = 0; i < n; i++)
        {
            if (events[i].events & EPOLLIN)
            {
                if (check_event(fifo_fd) == -1)
                {
                    return;
                }
            }
        }
    }
}
int check_event(int fifo_fd)
{
    char buffer[READ_SIZE];
    int count = read(fifo_fd, buffer, READ_SIZE - 1);
    if (count == -1)
    {
        return -1;
    }
    buffer[count] = '\0';
    if (strcmp(buffer, "ping") == 0 || strcmp(buffer, "ping\n") == 0)
    {
        printf("pong!\n");
    }
    else if (strcmp(buffer, "pong") == 0 || strcmp(buffer, "pong\n") == 0)
    {
        printf("ping!\n");
    }
    else if (strcmp(buffer, "quit") == 0 || strcmp(buffer, "quit\n") == 0)
    {
        printf("quit\n");
        return -1;
    }
    else
    {
        printf("Unknown: %s\n", buffer);
    }

    return 0;
}