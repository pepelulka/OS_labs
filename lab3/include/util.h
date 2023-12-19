#pragma once

#ifdef _WIN32

#include <windows.h>

#include <iostream>
#include <vector>
#include <cstdint>
#include <random>

namespace util {

// Writing to mmap for parent process
class TMMap {
private:
    std::string pname, pmutexName;
    size_t psize, bufferSize;
    HANDLE mmap;
    // View starts with 4 bytes of header and then data
    void * view;
    // If true - reading / else - writing
    bool readingMode;
    // Reading:
    size_t bytesRead = 0;
    // Writing:
    size_t bytesWritten = 0;
public:
    constexpr const static size_t DEFAULT_MAP_SIZE = 1024; 
    constexpr const static size_t HEADER_SIZE = 4;

    // mode == 'r' or mode == 'w'
    TMMap(
        char mode,
        const std::string& mmapName,
        size_t mmapSize = DEFAULT_MAP_SIZE
    );

    TMMap() = delete;
    TMMap(const TMMap&) = delete;

    ~TMMap();

    uint32_t GetBytesCount();
    void Write(const std::string& line);
    int Getc();
};

class Process {
private:
    PROCESS_INFORMATION info;
public:
    Process() = delete;
    Process(const Process&) = delete;
    Process(const std::string &path, const std::vector<std::string> &args);
    ~Process();

    void Terminate();
    void Wait();
};

class Randomizer {
private:
    std::mt19937 rng;
    std::uniform_int_distribution<std::mt19937::result_type> dist;
public:
    Randomizer() {
        std::random_device dev;
        rng = std::mt19937(dev());
        dist = std::uniform_int_distribution<std::mt19937::result_type>(1, 100);
    }

    bool Probability(size_t percentage) {
        return dist(rng) <= percentage;
    }
};

bool IsVowel(char c);

}

#endif
