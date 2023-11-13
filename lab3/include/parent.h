#pragma once

#ifdef _WIN32

#include <iostream>

#include <windows.h>

namespace lab3 {

void ParentRoutine(const std::string& pathToChild, std::istream &in);

}

#endif
