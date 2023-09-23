#include "utils.h"

#include <stdio.h>


int main(int argc, char** argv) {
    if (argc < 2) {
        printf("Missing arguments!\n");
    }
    char* input;
    while ((input = ReadString(stdin)) != NULL) {
        printf("---------\n");
        printf("%s %s\n", input, argv[1]);
        printf("---------\n");

        free(input);
    }
}