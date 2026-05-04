#include <iostream>
#include <vector>
#include <cassert>
#include <algorithm>
#include "sorts.h"

void runTests(void (*sortFunc)(std::vector<int>&), const std::string& sortName) {
    std::cout << "Running: " << sortName << "..." << std::endl;

    std::vector<int> v1 = { 5, 2, 9, 1, 5, 6 };
    sortFunc(v1);
    assert(std::is_sorted(v1.begin(), v1.end()));

    std::vector<int> v2 = {};
    sortFunc(v2);
    assert(v2.empty());

    std::vector<int> v3 = { -10, 5, -20, 0, 3 };
    sortFunc(v3);
    assert(std::is_sorted(v3.begin(), v3.end()));

    std::vector<int> v4 = { 1000000, -1000000, 500, 0 };
    sortFunc(v4);
    assert(std::is_sorted(v4.begin(), v4.end()));

    std::vector<int> v5 = { 10, 10, 10, 10, 10};
    sortFunc(v5);
    assert(std::is_sorted(v4.begin(), v4.end()));

}

int main() {
    runTests(bubbleSort, "Bubble Sort");
    runTests(heapSort, "Heap Sort");

    std::cout << "success" << std::endl;
    return 0;
}
