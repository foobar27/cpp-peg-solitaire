#include <string>
#include <algorithm>
#include <locale>
#include <iostream>
#include <boost/range/adaptors.hpp>
#include <boost/range/algorithm.hpp>
#include <boost/format.hpp>

using namespace std;

int main(int argc, char** argv) {
  string line { "hello w\torld"};
  line.erase(remove_if(line.begin(), line.end(), [](char c) { return isspace(c, locale()); }), line.end());
  cout << line << endl;
  return 0;
}
