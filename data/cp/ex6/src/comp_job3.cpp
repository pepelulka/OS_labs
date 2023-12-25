#include <fstream>
#include <vector>
#include <algorithm>

int main() {
    std::ifstream in("temp3.txt");
    size_t n;
    in >> n;
    std::vector<int> a;
    for (size_t i = 0; i < n; i++) {
        int cur;
        in >> cur;
    }
    std::sort(a.begin(), a.end());
    in.close();
    std::ofstream out("temp3.txt");
    out << a.size() << " ";
    for (auto i : a) {
        out << i << " ";
    }
}