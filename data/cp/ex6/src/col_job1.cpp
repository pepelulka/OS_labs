#include <fstream>
#include <vector>

// col_job1

int main() {
    std::ifstream in("input.txt");
    size_t n;
    in >> n;
    std::vector<int> a;
    for (size_t i = 0; i < n; i++) {
        int cur;
        in >> cur;
        if (cur <= 1000) {
            a.push_back(cur);
        }
    }
    std::ofstream out("temp1.txt");
    out << a.size() << " ";
    for (auto i : a) {
        out << i << " ";
    }
}