#include "../include/main.h"
#include "../include/tcp.h"
#include "../include/http.h"
#include "../include/route.h"

#include <cjson/cJSON.h>

typedef struct {
    short port;
} server_config;

int loadConfig(server_config *config) {
    char *configdata = loadfile("config.json");
    if (!configdata) {
        debug_log("Failed to read config.json file");
    }

    cJSON *config_json = cJSON_Parse(configdata);
    free(configdata);

    if (config_json == NULL) {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL) {
            fprintf(stderr, "JSON Parse Error before: %s\n", error_ptr);
        }
        return 0;
    }

    cJSON *port = cJSON_GetObjectItemCaseSensitive(config_json, "portnumber");
    if (!cJSON_IsNumber(port)) {
        cJSON_Delete(config_json);
        return 0;
    }

    if (port->valueint > 65535 || port->valueint < 0) {
        debug_log("Invalid port number specified in config");
        cJSON_Delete(config_json);
        return 0;
    }

    config->port = (short)port->valueint;
    cJSON_Delete(config_json);
    return 1;
}

int main() {
    tcp_server server = {0};

    server_config config = {
        .port = 8080,
    };

    if (!loadConfig(&config)) {
        debug_log("Failed to load config, using default values");
    }

    server_status_e status = bind_tcp_port(&server, config.port);
    if (status != SERVER_OK) {
        debug_log("Server initialization failed");
        exit(EXIT_FAILURE);
    }

    for (;;) {
        int client_fd = accept_client(server.socket_fd);
        if (client_fd == -1) {
            debug_log("Failed to accept client connection");
            continue;
        }

        debug_log("Client connected");

        http_request request = {0};
        http_response response = {0};

        init_http_response(&response);

        if (read_http_request(client_fd, &request) != HTTP_PARSE_OK) {
            debug_log("Failed to read HTTP request");
            free_http_headers(&request);
            free_http_response(&response);
            close(client_fd);
            continue;
        }

        if (parse_http_headers(request.buffer, &request) != HTTP_PARSE_OK) {
            debug_log("Failed to parse HTTP headers");
            free_http_headers(&request);
            free_http_response(&response);
            close(client_fd);
            continue;
        }

        char sanitized_path[1024] = {0};
        sanitize_path(request.path, sanitized_path, sizeof(sanitized_path));

        if (!handle_request(&request, &response)) {
            serve_file(sanitized_path, &response);
        }

        send_http_response(client_fd, &response);
     
        free_http_headers(&request);
        free_http_response(&response);    

        close(client_fd);
    }
    
    close(server.socket_fd);
    return 0;
}