#include "manager_base.hpp"

using namespace lab5;

int main() {
    if (getenv("PATH_TO_COMP") == nullptr) {
        std::cerr << "PATH_TO_COMP is not specified" << std::endl;
        exit(EXIT_FAILURE);
    }
    std::string path = std::string(getenv("PATH_TO_COMP"));
    TFrontend front;
    TMainManager man(path, front);
    std::thread t1(&TFrontend::Routine, &front);
    std::thread t2(&TMainManager::Routine, &man);
    std::thread t3(&TSink::Routine, &man.sink);
    t1.join();
    t2.join();
    t3.join();
}
