#include "util.h"

#include <iostream>
#include <fstream>
#include <cassert>

int main(int argc, char **argv) {
    if (argc < 3) {
        std::cout << "Too few arguments\n";
        exit(1);
    }
    std::ofstream out(argv[1]);
    util::TMMap map('r', argv[2]);
    for (size_t i = 0; i < map.GetBytesCount(); i++) {
        char c = map.Getc();
        if (!util::IsVowel(c)) {
            out << c;
        }
    }
    out.close();
}
