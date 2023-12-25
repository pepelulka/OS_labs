#include <fstream>
#include <vector>
#include <iostream>

// col_job2

int main() {
    std::ifstream in("input.txt");
    size_t n;
    in >> n;
    std::vector<int> a;
    for (size_t i = 0; i < n; i++) {
        int cur;
        in >> cur;
        std::cout << "c2 : " << cur << std::endl;
        if (cur > 1000 && cur < 2000) {
            a.push_back(cur);
        }
    }
    std::ofstream out("temp2.txt");
    out << a.size() << " ";
    for (auto i : a) {
        out << i << " ";
    }
}