#include "Board.hpp"

#include <algorithm>
#include <string>
#include <sstream>

using namespace std;

namespace pegsolitaire {

  ParseException::ParseException(int m_row, int m_column, const std::string & message)
    : std::runtime_error(message)
    , m_row(m_row)
    , m_column(m_column)
  {}

  int ParseException::row() const {
    return m_row;
  }

  int ParseException::column() const {
    return m_column;
  }

  Masks::Masks(int size)
    : moveMask(false)
    , checkMask1(false)
    , checkMask2(false)
  {
    assert(size < 64);
  }

}
