#include "HashSet.hpp"

#include <unordered_set>
#include <iostream>
#include <chrono>
#include <sstream>
#include <boost/program_options.hpp>

namespace po = boost::program_options;

using namespace std;
using namespace pegsolitaire;

int main(int argc, char** argv) {

    po::options_description desc("Allowed options");
    desc.add_options()
            ("help", "produce help message")
            ("number-of-elements", po::value<unsigned long>()->default_value(1000000), "number of elements")
            ("number-of-iterations", po::value<unsigned long>()->default_value(10), "number of iterations")
            ;

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help")) {
        cout << desc << "\n";
        return 1;
    }

    unsigned long numberOfIterations = vm["number-of-iterations"].as<unsigned long>();
    unsigned long numberOfElements = vm["number-of-elements"].as<unsigned long>();

    HashSet< HashSetTraits<32> > s;
    unordered_set<decltype(*s.begin())> duplicates;
    unsigned long expected = 0;
    using valueType = decltype(*s.begin());
    for (unsigned long idx = 0; idx < numberOfElements; ++idx) {
        valueType v = idx*idx + 1;
        if (v != 0 && duplicates.count(v) == 0) {
            s += v;
            for (int it = 0; it < numberOfIterations; ++it)
                expected ^= v;
            duplicates.insert(v);
        } // else: just skip 'invalidElement'
    }

    {
        cout << "starting benchmark (via iterator)..." << endl;

        auto begin = std::chrono::high_resolution_clock::now();
        unsigned long actual = 0;
        for (unsigned long i = 0; i < numberOfIterations; ++i)
            for (auto v : s)
                actual ^= v;
        auto end = std::chrono::high_resolution_clock::now();

        cout << "benchmark finished. value=" << actual << endl;

        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin);
        if (actual != expected) {
            cerr << "self-check failed, expected " << actual << endl
                 << "                    but got " << actual << endl;
            return -1;
        }
        cout << "elapsed (via iterator): " << elapsed.count() << " ms" << endl;
    }

    {
        cout << "starting benchmark (via raw data)..." << endl;

        auto begin = std::chrono::high_resolution_clock::now();
        unsigned long actual = 0;
        for (unsigned long i = 0; i < numberOfIterations; ++i) {
            auto p = s.rawData();
            unsigned long sz = s.capacity();
            for (int idx=0; idx<sz; ++idx, ++p) {
                if (*p != 0)
                    actual ^= *p;
            }
        }
        auto end = std::chrono::high_resolution_clock::now();

        cout << "benchmark finished. value=" << actual << endl;

        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin);
        if (actual != expected) {
            cerr << "self-check failed, expected " << expected << endl
                 << "                    but got " << actual << endl;
            return -1;
        }
        cout << "elapsed (via raw data): " << elapsed.count() << " ms" << endl;
    }

    return 0;
}
