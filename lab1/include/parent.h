#ifndef PARENT_H
#define PARENT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include "utils.h"

void ParentRoutine(char* childProgramPath, FILE* stream);

#endif //PARENT_H
