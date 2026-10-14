#include "../include/route.h"
#include "../include/http.h"

Route routes[MAX_ROUTES];
int route_count = 0;

size_t install_route(http_method_e methode, const char *path, void (*handler)(http_request *request, http_response *response)) {
    if (route_count < MAX_ROUTES) {
        routes[route_count].methode = methode;
        strncpy(routes[route_count].path, path, sizeof(routes[route_count].path) - 1);
        routes[route_count].path[sizeof(routes[route_count].path) - 1] = '\0';
        routes[route_count].handler = handler;
        return ++route_count;
    }

    return route_count;
}