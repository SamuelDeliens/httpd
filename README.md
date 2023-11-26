# httpd
project server httpd

- - - -

### subjects
[step 1](https://github.com/SamuelDeliens/httpd/blob/main/httpd-step1-subject.pdf "Named link title") </br>
[step 2](https://github.com/SamuelDeliens/httpd/blob/main/httpd-step2-subject.pdf "Named link title")

[config](https://github.com/SamuelDeliens/httpd/blob/main/httpd-config.pdf "Named link title") </br>
[epoll](https://github.com/SamuelDeliens/httpd/blob/main/httpd-epoll.pdf "Named link title")

- - - -

## Introduction

The HTTPd project is aimed at developing an HTTP/1.1 server in accordance with the HTTP/1.1 specification detailed in various RFCs. The project's focus is on understanding and implementing the protocol's requirements, with an emphasis on running the server as a daemon.

## Architecture

The project is modular, with each part contributing to the overall functionality of the HTTP server. Key components include:

- Config Module: Handles loading and managing configuration files.
- Utils Module: Provides utility functions, particularly for string handling.
- HTTP Module: Contains all HTTP-related functionalities, such as parsing requests and creating responses.
- Daemon Module: Assists in daemonizing the program, handling process forking, and signal management.
- Server Module: Manages the creation and execution of the server, including socket preparation and the accept loop.
- Logger Module: Acts as the logger for the server, aiding in debugging network interactions.

## Core Features

1. Configuration: Ability to parse configuration files and handle global settings and virtual hosts.
2. Basic Server Functionality: Including network programming aspects and an echo server setup for initial testing.
3. Daemon Control: Implementation of daemon processes with start, stop, reload, and restart capabilities.
4. HTTP 1.1 Support: Handling HTTP requests and responses following the HTTP/1.1 standards.
5. Error Handling: Robust management of HTTP errors and status codes.
6. Logging: Detailed logging of requests and responses for debugging and monitoring.

## Usage

To use the HTTPd server, follow these steps:

1. Compile the project using the provided Makefiles.
2. Configure the server using the server.conf file, specifying global settings and virtual hosts.
3. Start the server using the command: ./httpd [-options] server.conf. Options include --dry-run for testing configurations and -a [action] for daemon control actions like start, stop, reload, and restart.


## Testing

The project includes unit tests and supports testing using tools like cURL and netcat. It's recommended to create a comprehensive testsuite to ensure all functionalities work as expected.

This README provides a high-level overview of your project. For detailed instructions and a complete understanding of each component, users should refer to the specific sections of the project documentation.
