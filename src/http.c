#include "../include/http.h"

#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdbool.h>

http_parse_e parse_http_headers(const char *raw_request, http_request *request) {
    const char *line_start = strstr(raw_request, "\r\n");
    if (!line_start) {
        return HTTP_PARSE_INVALID;
    }

    line_start += 2;
    while (line_start && *line_start && *line_start != '\r' && *line_start != '\n') {
        const char *line_end = strstr(line_start, "\r\n");
        if (!line_end) {
            break;
        }

        size_t line_length = line_end - line_start;
        char line[1024] = {0};
        strncpy(line, line_start, line_length);

        char *colon_pos = strchr(line, ':');
        if (colon_pos) {
            *colon_pos = '\0';
            const char *key = line;
            const char *value = colon_pos + 1;

            while (*value == ' ') {
                value++;
            }

            request->headers = realloc(request->headers, sizeof(http_header_t) * (request->header_count + 1));
            if (!request->headers) {
                perror("Failed to allocate memory for headers");
                exit(EXIT_FAILURE);
            }

            strncpy(request->headers[request->header_count].key, key, sizeof(request->headers[request->header_count].key) - 1);
            strncpy(request->headers[request->header_count].value, value, sizeof(request->headers[request->header_count].value) - 1);

            request->header_count++;
        }

        line_start = line_end + 2;
    }

    return HTTP_PARSE_OK;
}

void free_http_headers(http_request *request) {
    free(request->headers);
    request->headers = NULL;
    request->header_count = 0;
}

http_parse_e read_http_request(int socket_fd, http_request *request) {
    ssize_t bytes_read = read(socket_fd, request->buffer, sizeof(request->buffer) - 1);
    
    if (bytes_read <= 0) {
        return HTTP_PARSE_INVALID;
    }

    request->buffer[bytes_read] = '\0';

    if (sscanf(request->buffer, "%7s %2047s %15s", request->method, request->path, request->protocol) != 3) {
        return HTTP_PARSE_INVALID;
    }

    return HTTP_PARSE_OK;
}