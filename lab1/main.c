#include "parent.h"

#include <time.h>

int main() {
    srand(time(NULL));
    char* childProgramName;
    if ((childProgramName = getenv("PATH_TO_CHILD")) == NULL) {
        childProgramName = "./child";
    }
    ParentRoutine(childProgramName, stdin);
}
