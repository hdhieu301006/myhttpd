#include "../include/tcp.h"
#include "../include/util.h"

server_status_e bind_tcp_port(tcp_server *server, int port) {
    if (server == NULL) {
        return SERVER_SOCKET_ERROR;
    }

    if (port < 0 || port > 65535) {
        debug_log("Invalid port number");
        return SERVER_BIND_ERROR;
    }

    memset(server, 0, sizeof(*server));

    server->socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server->socket_fd < 0) {
        debug_log("Socket creation failed");
        return SERVER_SOCKET_ERROR;
    }

    int opt = 1;
    setsockopt(server->socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    server->address.sin_family = AF_INET;
    server->address.sin_addr.s_addr = INADDR_ANY;
    server->address.sin_port = htons((uint16_t)port);

    if (bind(server->socket_fd, (struct sockaddr *)&(server->address), sizeof(server->address)) < 0) {
        debug_log("Bind failed");
        close(server->socket_fd);
        server->socket_fd = -1;
        return SERVER_BIND_ERROR;
    }

    socklen_t addr_len = sizeof(server->address);
    getsockname(server->socket_fd, (struct sockaddr *)&(server->address), &addr_len);

    if (listen(server->socket_fd, 5) < 0) {
        debug_log("Listen failed");
        close(server->socket_fd);
        server->socket_fd = -1;
        return SERVER_LISTEN_ERROR;
    }

    debug_log("Server bound and listening");
    return SERVER_OK;
}

int accept_client(int server_fd) {
    if (server_fd < 0) {
        return -1;
    }
    
    struct sockaddr_in client_address;
    socklen_t client_len = sizeof(client_address);
    memset(&client_address, 0, sizeof(client_address));
    
    int client_fd = accept(server_fd, (struct sockaddr *)&client_address, &client_len);
    if (client_fd < 0) {
        debug_log("Accept failed");
        return -1;
    }

    return client_fd;
}