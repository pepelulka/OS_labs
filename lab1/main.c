#include "parent.h"

#include <time.h>

int main() {
    srand(time(NULL));
    ParentRoutine("./child");
}