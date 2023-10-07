#include "utils.h"

int main(int argc, char** argv) {
    if (argc < 2) {
        printf("Missing arguments!\n");
        exit(EXIT_FAILURE);
    }

    char* filename = argv[1];
    FILE* file = fopen(filename, "w");
    if (file == NULL) {
        printf("Can't open file %s\n", filename);
        exit(EXIT_FAILURE);
    }

    char* input;
    while ((input = ReadStringAndRemoveVowels(stdin)) != NULL) {
        fprintf(file, "%s", input);
        // fflush(file);

        free(input);
    }

    fclose(file);
}
