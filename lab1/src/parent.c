#include "parent.h"

void ParentRoutine(char* childProgramPath) {
    char fileName1[128], fileName2[128];

    scanf("%s\n", fileName1);
    scanf("%s\n", fileName2);

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
    while ((input = ReadString(stdin)) != NULL) {
        if (Probability(80)) {
            write(pipe1[PIPE_WRITE], input, strlen(input));
        } else {
            write(pipe2[PIPE_WRITE], input, strlen(input));
        }

        free(input);
    }

    close(pipe1[PIPE_WRITE]);
    close(pipe2[PIPE_WRITE]);
}
