#include <iostream>
#include <unordered_map>
#include <chrono>
#include <set>
#include <unordered_set>

#include "Tree.h"

int main() {
    using namespace std::chrono;
    Tree<uint64_t, 37> tree;
    std::vector<uint64_t> values;

    // generate 1million distinct random integer
    for(int i = 0; i < 1000000; i++) {
        uint64_t value = rand() & 0xFFFFFFFF;
        while(tree.member(value)) {
            value = rand() & 0xFFFFFFFF;
        }
        tree.insert(value);
        values.push_back(value);
    }

    auto start = high_resolution_clock::now();
    std::sort(values.begin(), values.end());
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
    std::cout << "std::sort: " << duration.count() << " microseconds" << std::endl;


    std::optional<uint64_t> current = 0;
    start = high_resolution_clock::now();
    while(current.has_value()) {
        current = tree.succ(current.value());
    }
    stop = high_resolution_clock::now();
    duration = duration_cast<microseconds>(stop - start);
    std::cout << "Tree::succ::sort: " << duration.count() << " microseconds" << std::endl;



    // set operations
    std::set<uint64_t> keys;
    for(auto value : values) {
        keys.insert(value);
    }

    start = high_resolution_clock::now();
    for(int i=0; i<1000000; i++) {
        keys.find(i);
    }
    stop = high_resolution_clock::now();
    duration = duration_cast<microseconds>(stop - start);
    std::cout << "std::set::find: " << duration.count() << " microseconds" << std::endl;

    start = high_resolution_clock::now();
    for(int i=0; i<1000000; i++) {
        tree.member(i);
    }
    stop = high_resolution_clock::now();
    duration = duration_cast<microseconds>(stop - start);
    std::cout << "Tree::member: " << duration.count() << " microseconds" << std::endl;

    start = high_resolution_clock::now();
    for(uint64_t i: values) {
        keys.erase(i);
    }
    stop = high_resolution_clock::now();
    duration = duration_cast<microseconds>(stop - start);
    std::cout << "std::set::erase: " << duration.count() << " microseconds" << std::endl;

    start = high_resolution_clock::now();
    for(uint64_t x: values) {
        tree.remove(x);
    }
    stop = high_resolution_clock::now();
    duration = duration_cast<microseconds>(stop - start);
    std::cout << "Tree::remove: " << duration.count() << " microseconds" << std::endl;

}
