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
void CreateChildForPipe(char* fileName, int pipe[2], char** args);

bool IsVowel(char c);
bool Probability(int percentage);

char* ReadString(FILE* stream);
char* ReadStringAndRemoveVowels(FILE* stream);

#endif //UTILS_H
