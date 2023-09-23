#include "utils.h"

void CreatePipe(int pipeFd[2]) {
    if(pipe(pipeFd) != 0) {
        printf("Couldn't create pipe\n");
        exit(EXIT_FAILURE);
    }
}

char *ReadString(FILE *stream) {
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

    char cur;
    while ((cur = getc(stream)) != EOF) {
        buffer[idx++] = cur;

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

    buffer[idx] = '\n';

    return buffer;
}

bool Probability(int percentage) {
    assert(0 <= percentage && percentage <= 100);
    return (rand() % 100) < percentage;
}

void CreateChildForPipe(char* fileName, int pipe[2], char** args) {
    int pid = fork();
    if (pid == -1) {
        printf("Can't fork!\n");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        // close: decrement reference count of file descriptor in kernel
        close(pipe[PIPE_WRITE]);
        // dup2: pipe[PIPE_READ] has an alias '0' ans '0' file descriptor is closed
        dup2(pipe[PIPE_READ], 0);
        execv(fileName, args);
    }
}
