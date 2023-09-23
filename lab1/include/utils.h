#ifndef UTILS_H
#define UTILS_H

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

enum {
    PIPE_READ,
    PIPE_WRITE
};

void CreatePipe(int pipeFd[2]);
char* ReadString(FILE* stream);
bool Probability(int percentage);
void CreateChildForPipe(char* fileName, int pipe[2], char** args);

#endif //UTILS_H
