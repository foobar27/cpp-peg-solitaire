#include "BoardIO.hpp"

#include <algorithm>
#include <locale>
#include <sstream>
#include <numeric>
#include <utility>

namespace pegsolitaire {

  using namespace std;

  enum class Field : char { BLOCKED = 'o', EMPTY = '.', PEG };

  locale loc;

  vector<string> readLinesUntilBlank(istream & is) {
    vector<string> lines;
    while (!is.eof()) {
      string line;
      getline(is, line);
      if (!any_of(line.begin(),
                  line.end(),
                  [](char c) { return !isspace(c, loc); }))
        break; // stop on blank string
      getline(is, line);
    }
    return lines;
  }

  // TODO test
  Matrix<bool> parseLines(const vector<string> & lines) {
    vector<vector<bool>> parsed;
    int lineNumber = 0;
    int lineLength = -1;
    for (const auto & line : lines) {
      vector<bool> l;
      int columnNumber = 0;
      for (char c : line) {
        if (isspace(c, loc))
          continue;
        switch (static_cast<Field>(c)) {
        case Field::BLOCKED:
          l.push_back(false);
          break;
        case Field::EMPTY:
          l.push_back(true);
          break;
        default: {
          stringstream ss;
          ss << "invalid character: '" << c << "'";
          throw ParseException(lineNumber, columnNumber, ss.str());
        }
        }
        columnNumber++;
      }
      if (!l.empty()) {
        if (lineLength < 0)
          lineLength = l.size();
        if (l.size() != static_cast<size_t>(lineLength))
          throw ParseException(lineNumber, line.length() - 1, "all lines must have equal length!");
        parsed.push_back(l);
      }
      lineNumber++;
    }
    if (parsed.empty())
      throw ParseException(0, 0, "empty board!");
    return parsed;
  }
}
