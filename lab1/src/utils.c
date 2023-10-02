#include "utils.h"

void CreatePipe(int pipeFd[2]) {
    if(pipe(pipeFd) != 0) {
        printf("Couldn't create pipe\n");
        exit(EXIT_FAILURE);
    }
}

void CreateChildForPipe(char* fileName, int pipe[2], char** args) {
    int pid = fork();
    if (pid == -1) {
        printf("Can't fork!\n");
        exit(EXIT_FAILURE);
    }
    if (pid == 0) {
        close(pipe[PIPE_WRITE]);
        dup2(pipe[PIPE_READ], 0);
        execv(fileName, args);
    }
}

bool IsVowel(char c) {
    return c == 'e' ||
           c == 'u' ||
           c == 'i' ||
           c == 'o' ||
           c == 'a';
}

bool Probability(int percentage) {
    assert(0 <= percentage && percentage <= 100);
    return (rand() % 100) < percentage;
}

char* ReadString(FILE *stream) {
    if (feof(stream)) {
        return NULL;
    }

    const size_t chunkSize = 256;
    size_t size = 256;
    char* buffer = (char*)malloc(size * sizeof(char));
    size_t idx = 0;

    if(!buffer) {
        printf("Couldn't allocate buffer");
        exit(EXIT_FAILURE);
    }

    int cur;
    while ((cur = getc(stream)) != EOF) {
        buffer[idx++] = (char)cur;

        if (idx == size) {
            size += chunkSize;
            buffer = realloc(buffer, size * sizeof(char));

            if(!buffer) {
                printf("Couldn't allocate buffer");
                exit(EXIT_FAILURE);
            }
        }

        if (cur == '\n') {
            break;
        }
    }

    buffer[idx] = '\0';

    return buffer;
}

char* ReadStringAndRemoveVowels(FILE* stream) {
    if (feof(stream)) {
        return NULL;
    }

    const size_t chunkSize = 256;
    size_t size = 256;
    char *buffer = (char*)malloc(size * sizeof(char));
    size_t idx = 0;

    if(!buffer) {
        printf("Couldn't allocate buffer");
        exit(EXIT_FAILURE);
    }

    int cur;
    while ((cur = getc(stream)) != EOF) {
        if (!IsVowel(cur)) {
            buffer[idx++] = cur;
        }

        if (idx == size) {
            size += chunkSize;
            buffer = realloc(buffer, size * sizeof(char));

            if(!buffer) {
                printf("Couldn't allocate buffer");
                exit(EXIT_FAILURE);
            }
        }

        if (cur == '\n') {
            break;
        }
    }

    buffer[idx] = '\0';


    return buffer;
}

