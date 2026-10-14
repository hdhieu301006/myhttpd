#ifndef ROUTE_H
#define ROUTE_H

#include "../include/http.h"
#include <string.h>

#define MAX_ROUTES 100

typedef struct {
    http_method_e methode;
    char path[128];
    void (*handler)(http_request *, http_response *);
} Route;

size_t install_route(http_method_e methode, const char *path, void (*handler)(http_request *request, http_response *response));
#endif
