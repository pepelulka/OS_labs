#ifdef __unix__

extern "C" {
    #include "lib.h"
}

#include <dlfcn.h>

#include <iostream>
#include <vector>
#include <array>
#include <sstream>

// MainRoutine pipeline :
//  |   1. Start() runs event loop and get line.
//  |   2. ProceedLine(line) parse string and decide what function to call.
//  |       It uses Tokenize(line) to divide string into words.
//  |   3. Call handlers (Help(), Exit() ...)
// \./
class TMainRoutine {
private:

    // Contracts:
    using Function1Ptr = float(*)(float, float);
    using Function2Ptr = int*(*)(int*, size_t);

    const char * Function1Name = "Derivative";
    const char * Function2Name = "Sort";
    //

    Function1Ptr func1;
    Function2Ptr func2;
    size_t currentMode;
    std::array<void*, 2> dls;

    // Handlers =========================================================

    void SwitchMode() {
        currentMode = 1 - currentMode;
        func1 = (Function1Ptr)(dlsym(dls[currentMode], Function1Name));
        func2 = (Function2Ptr)(dlsym(dls[currentMode], Function2Name));
    }

    void InvalidArgs() {
        std::cout << "Invalid arguments\n";
    }

    void MissingArgs() {
        std::cout << "Missing arguments\n";
    }

    void Function1(float a, float deltaX) {
        std::cout << func1(a, deltaX) << std::endl;
    }

    void Function2(int * array, size_t size) {
        func2(array, size);
        for (size_t i = 0; i < size; i++) {
            std::cout << array[i] << " ";
        }
        std::cout << std::endl;
    }

    void Help() {
        std::cout << "I can do:\n";
        std::cout << "  1. help\n";
        std::cout << "  2. exit - exits program\n";
        std::cout << "  3. 1 <a> <deltaX> - calculates derivative of cos(x) of a with specific deltaX\n";
        std::cout << "  4. 2 <size> <a0> <a1> ... <an> - sort array a\n";
    }

    // Handlers end =====================================================

    std::vector<std::string> Tokenize(const std::string &line) {
        std::vector<std::string> result;
        std::stringstream ss(line);
        while (ss) {
            std::string token;
            ss >> token;
            if (token != "") {
                result.push_back(token);
            }
        }
        return result;
    }

    // Return false if user exited
    bool ProceedLine(const std::string &line) {
        std::vector<std::string> tokens = Tokenize(line);
        if (tokens[0] == "help") {
            Help();
        } else if (tokens[0] == "exit") {
            return false;
        } else if (tokens[0] == "1") {
            try {
                if (tokens.size() < 3) {
                    MissingArgs();
                    return true;
                }
                Function1(std::atof(tokens[1].c_str()), std::atof(tokens[2].c_str()));
            } catch (...) {
                InvalidArgs();
            }
        } else if (tokens[0] == "2") {
            try {
                if (tokens.size() < 2) {
                    MissingArgs();
                    return true;
                }
                int size = std::atoi(tokens[1].c_str());
                if (size < 0) {
                    InvalidArgs();
                    return true;
                }
                if (tokens.size() < static_cast<size_t>(2 + size)) {
                    MissingArgs();
                    return true;
                }
                int* ptr = new int[size];
                for (int i = 0; i < size; i++) {
                    ptr[i] = std::atoi(tokens[i + 2].c_str());
                }
                Function2(ptr, size);
                delete[] ptr;
            } catch (...) {
                InvalidArgs();
            }
        } else if (tokens[0] == "0") {
            SwitchMode();
            std::cout << "Switched\n";
        } else {
            InvalidArgs();
        }
        return true;
    }
public:
    TMainRoutine() = delete;

    TMainRoutine(const char * lib1Path, const char * lib2Path) {
        dls[0] = dlopen(lib1Path, RTLD_LOCAL | RTLD_LAZY);
        if (dls[0] == nullptr) {
            throw std::invalid_argument("Can't open first lib.");
        }
        dls[1] = dlopen(lib2Path, RTLD_LOCAL | RTLD_LAZY);
        if (dls[1] == nullptr) {
            throw std::invalid_argument("Can't open second lib.");
        }
        currentMode = 1;
        SwitchMode();
    }

    void Start() {
        bool terminated = false;
        do {
            std::cout << "> ";
            std::string line;
            std::getline(std::cin, line);
            terminated = !ProceedLine(line);
        } while (!terminated);
    }
};

#endif

int main() {
    if (getenv("PATH_TO_LIB1") == nullptr) {
        std::cout << "PATH_TO_LIB1 is not specified\n";
        exit(1);
    }
    if (getenv("PATH_TO_LIB2") == nullptr) {
        std::cout << "PATH_TO_LIB2 is not specified\n";
        exit(1);
    }
    TMainRoutine routine(getenv("PATH_TO_LIB1"), getenv("PATH_TO_LIB2"));
    routine.Start();
}
