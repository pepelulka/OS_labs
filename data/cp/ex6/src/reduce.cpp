#include <fstream>
#include <vector>

// col_job1

int main() {
    std::ifstream in1("temp1.txt");
    std::ifstream in2("temp2.txt");
    std::ifstream in3("temp3.txt");
    std::vector<int> a;
    size_t n;
    in1 >> n;
    for (size_t i = 0; i < n; i++) {
        int cur;
        in1 >> cur;
        a.push_back(cur);
    }

    in2 >> n;
    for (size_t i = 0; i < n; i++) {
        int cur;
        in2 >> cur;
        a.push_back(cur);
    }

    in3 >> n;
    for (size_t i = 0; i < n; i++) {
        int cur;
        in3 >> cur;
        a.push_back(cur);
    }

    std::ofstream out("output.txt");
    for (auto i : a) {
        out << i << " ";
    }
}