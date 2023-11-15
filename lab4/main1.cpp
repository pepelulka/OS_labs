extern "C" {
    #include "lib.h"
}

#include <iostream>
#include <sstream>
#include <vector>

// MainRoutine pipeline :
//  |   1. Start() runs event loop and get line.
//  |   2. ProceedLine(line) parse string and decide what function to call.
//  |       It uses Tokenize(line) to divide string into words.
//  |   3. Call handlers (Help(), Exit() ...)
// \./
class TMainRoutine {
private:
    // Handlers =========================================================

    static void InvalidArgs() {
        std::cout << "Invalid arguments\n";
    }

    static void MissingArgs() {
        std::cout << "Missing arguments\n";
    }

    static void Function1(float a, float deltaX) {
        std::cout << Derivative(a, deltaX) << std::endl;
    }

    static void Function2(int * array, size_t size) {
        Sort(array, size);
        for (size_t i = 0; i < size; i++) {
            std::cout << array[i] << " ";
        }
        std::cout << std::endl;
    }

    static void Help() {
        std::cout << "I can do:\n";
        std::cout << "  1. help\n";
        std::cout << "  2. exit - exits program\n";
        std::cout << "  3. 1 <a> <deltaX> - calculates derivative of cos(x) of a with specific deltaX\n";
        std::cout << "  4. 2 <size> <a0> <a1> ... <an> - sort array a\n";
    }

    // Handlers end =====================================================

    static std::vector<std::string> Tokenize(const std::string &line) {
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
    static bool ProceedLine(const std::string &line) {
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
        } else {
            InvalidArgs();
        }
        return true;
    }
public:
    static void Start() {
        bool terminated = false;
        do {
            std::cout << "> ";
            std::string line;
            std::getline(std::cin, line);
            terminated = !ProceedLine(line);
        } while (!terminated);
    }
};

int main() {
    TMainRoutine::Start();
}
