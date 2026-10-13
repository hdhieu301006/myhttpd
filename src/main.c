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

    for (;;) {
        int client_fd = accept_client(server.socket_fd);
        if (client_fd == -1) {
            debug_log("Failed to accept client connection");
            close(server.socket_fd);
            exit(EXIT_FAILURE);
        }

        debug_log("Client connected");

        http_request request = {0};
        http_response response = {0};

        init_http_response(&response);

        if (read_http_request(client_fd, &request) != HTTP_PARSE_OK) {
            debug_log("Failed to read or parse HTTP request");
            close(client_fd);
            return 0;
        }

        if (parse_http_headers(request.buffer, &request) != HTTP_PARSE_OK) {
            debug_log("Failed to read or parse HTTP request");
            close(client_fd);
            return 0;
        }

        char sanitized_path[1024] = {0};
        sanitize_path(request.path, sanitized_path, sizeof(sanitized_path));
        serve_file(sanitized_path, &response);

        send_http_response(client_fd, &response);
     
        free_http_headers(&request);
        free_http_response(&response);    

        close(client_fd);
    }
    close(server.socket_fd);
    return 0;
}