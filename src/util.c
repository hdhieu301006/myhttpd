#include "../include/util.h"

void debug_log(const char *message) {
    if (message != NULL) {
        printf("[LOG] %s\n", message);
    }
}