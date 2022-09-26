#include <stdio.h>
#include <stdlib.h>
#include "mystring.h"

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("USAGE: %s <string to split>\n", argv[0]);
        return 1;
    }
    strarray *result = strsplit(argv[1]);
    for (int i = 0; i < result->length; i++) {
        printf("%d: \"%s\"\n", i, result->data[i]);
        free(result->data[i]);
    }
    free(result->data);
    free(result);
    return 0;
}
