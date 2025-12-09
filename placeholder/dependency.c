#include "dependency.h"
#include <stddef.h>

int countDependencies(const char *expr) {
    if (expr == NULL) {
        return 0;
    }

    int count = 0;
    for (int i = 0; expr[i] != '\0'; ++i) {
        if (expr[i] == '-' || expr[i] == '/') {
            count++;
        }
    }
    return count;
}
