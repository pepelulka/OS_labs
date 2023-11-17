#include "util.h"

#ifdef _WIN32

namespace util {

TMMap::TMMap(
    char mode,
    const std::string& mmapName,
    size_t mmapSize
) : pname(mmapName), psize(mmapSize), bufferSize(psize + HEADER_SIZE) {
    if (mode == 'r') {
        readingMode = true;
    } else if (mode == 'w') {
        readingMode = false;
    } else {
        throw std::invalid_argument("Expected mode == 'r' or 'w'");
    }
    // Create mmap
    mmap = CreateFileMappingA(
        INVALID_HANDLE_VALUE,
        nullptr,
        PAGE_READWRITE,
        0,
        bufferSize,
        mmapName.c_str()
    );

    if (mmap == nullptr) {
        throw std::logic_error("Can't create mmap");
    }

    view = MapViewOfFile(mmap, FILE_MAP_ALL_ACCESS, 0, 0, bufferSize);
}

TMMap::~TMMap() {
    UnmapViewOfFile(view);
    CloseHandle(mmap);
}

uint32_t TMMap::GetBytesCount() {
    return *((uint32_t*)view);
}

void TMMap::Write(const std::string& line) {
    if (readingMode == true) {
        throw std::logic_error("Can't write in reading mode.");
    }
    if (bytesWritten + line.size() > psize) {
        throw std::overflow_error("Buffer overflow.");
    }
    uint32_t* uintView = (uint32_t*)view;
    char * bufferTop = (char*)view + HEADER_SIZE + bytesWritten;
    // !!! Don't copy \0
    memcpy(bufferTop, line.c_str(), line.size());
    bytesWritten += line.size();
    *uintView = bytesWritten; 
}

int TMMap::Getc() {
    if (readingMode == false) {
        throw std::logic_error("Can't read in writing mode.");
    }
    uint32_t bytesCount = *((uint32_t*)view);
    if (bytesRead == bytesCount) {
        return EOF;
    }
    char result = ((char*)view)[HEADER_SIZE + bytesRead];
    bytesRead++;
    return result;
}

Process::Process(const std::string &path, const std::vector<std::string> &args) {
    STARTUPINFOA si;
    ZeroMemory(&si, sizeof(si));
    ZeroMemory(&info, sizeof(info));
    std::string cmdArgs = path;
    for (const auto& arg : args) {
        cmdArgs += " " + arg;
    }
    char rawArgs[1024];
    strcpy(rawArgs, cmdArgs.c_str());
    if(!CreateProcessA(path.c_str(),
                       rawArgs,
                       nullptr,
                       nullptr,
                       TRUE,
                       0,
                       nullptr,
                       nullptr,
                       &si,
                       &info)) {
        throw std::logic_error("Can't create new process.");
        return ;
    }
}

void Process::Terminate() {
    CloseHandle(info.hProcess);
    CloseHandle(info.hThread);
}

Process::~Process() {
    Terminate();
}

void Process::Wait() {
    WaitForSingleObject(info.hProcess, INFINITE);
    Terminate();
}

bool IsVowel(char c) {
    return c == 'e' ||
           c == 'u' ||
           c == 'i' ||
           c == 'o' ||
           c == 'a';
}

}

#endif
