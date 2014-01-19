#include "HashSet.hpp"

#include <set>
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
  set<decltype(*s.begin())> duplicates;
  unsigned long expectedSumPerIteration = 0;
  for (unsigned long i = 0; i < numberOfElements; ++i) {
    decltype(*s.begin()) v = i*i + 1;
    if (v != 0 && duplicates.count(v) == 0) {
      s += v;
      expectedSumPerIteration += v;
      duplicates.insert(v);
    } // else: just skip 'invalidElement'
  }

  unsigned long expectedSum = expectedSumPerIteration * numberOfIterations;

  auto begin = std::chrono::high_resolution_clock::now();
  unsigned long sum = 0;
  for (unsigned long i = 0; i < numberOfIterations; ++i)
    for (auto v : s)
      sum += v;
  auto end = std::chrono::high_resolution_clock::now();

  auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin);
  if (sum != expectedSum) {
    cerr << "self-check failed, expected " << expectedSum << endl
         << "                    but got " << sum << endl;
    return -1;
  }
  cout << "elapsed: " << elapsed.count() << " ms" << endl;
  return 0;
}
