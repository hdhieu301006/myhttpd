#include "../include/main.h"
#include "../include/tcp.h"
#include "../include/http.h"

int main() {
    tcp_server server = {0};
    server_status_e status = bind_tcp_port(&server, 8080);
    if (status != SERVER_OK) {
        debug_log("Server initialization failed");
        exit(EXIT_FAILURE);
    }

    int client_fd = accept_client(server.socket_fd);
    if (client_fd == -1) {
        debug_log("Failed to accept client connection");
        close(server.socket_fd);
        exit(EXIT_FAILURE);
    }

    debug_log("Client connected");

    http_request req = {0};

    if (read_http_request(client_fd, &req) != HTTP_PARSE_OK) {
        debug_log("Failed to read or parse HTTP request");
        close(client_fd);
        return 0;
    }

    if (parse_http_headers(req.buffer, &req) != HTTP_PARSE_OK) {
        debug_log("Failed to read or parse HTTP request");
        close(client_fd);
        return 0;
    }

    printf("Parsed HTTP Headers:\n");
    for (size_t i = 0; i < req.header_count; i++) {
        printf("%s: %s\n", req.headers[i].key, req.headers[i].value);
    }

    free_http_headers(&req);

    close(client_fd);
    close(server.socket_fd);
    return 0;
}