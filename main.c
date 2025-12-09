#include <stdio.h>
#include "dependency.h"

int main() {
    char expr[256];

    printf("Enter PREFIX expression:\n");
    if (scanf("%255s", expr) != 1) {
        return 1;
    }

    int deps = countDependencies(expr);
    printf("Dependency count: %d\n", deps);

    return 0;
}
