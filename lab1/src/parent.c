#include "parent.h"

void ParentRoutine(char* childProgramPath, FILE* stream) {
    char fileName1[128], fileName2[128];

    char *input1 = ReadString(stream);
    char *input2 = ReadString(stream);
    if (input1 == NULL || input2 == NULL) {
        printf("Error with input.\n");
        exit(EXIT_FAILURE);
    }

    strcpy(fileName1, input1);
    strcpy(fileName2, input2);
    free(input1);
    free(input2);

    int pipe1[2], pipe2[2];

    CreatePipe(pipe1);
    CreatePipe(pipe2);

    char* args1[] = {childProgramPath, fileName1, NULL};
    char* args2[] = {childProgramPath, fileName2, NULL};

    CreateChildForPipe(childProgramPath, pipe1, args1);
    CreateChildForPipe(childProgramPath, pipe2, args2);

    close(pipe1[PIPE_READ]);
    close(pipe2[PIPE_READ]);

    // Now parent's program:

    char* input;
    while ((input = ReadString(stream)) != NULL) {
        if (Probability(80)) {
            write(pipe1[PIPE_WRITE], input, strlen(input));
        } else {
            write(pipe2[PIPE_WRITE], input, strlen(input));
        }

        free(input);
    }

    close(pipe1[PIPE_WRITE]);
    close(pipe2[PIPE_WRITE]);
    wait(NULL);
}
