#pragma once

#include "PegSolitaireConfig.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>
#include <boost/dynamic_bitset.hpp>
#include "Matrix.hpp"

namespace pegsolitaire {

  class PEGSOLITAIRE_EXPORT ParseException : public std::runtime_error {
  public:
    ParseException(int row, int column, const std::string & message);
    int row() const;
    int column() const;
  private:
    const int m_row, m_column;
  };

  enum class MoveDirection { HORIZONTAL, VERTICAL, LEFT_DIAGONAL, RIGHT_DIAGONAL };

  /**
   * Store the 3 required bit masks, to detect if a move is possible and to execute it.
   *
   * (m,_,_) => ...111... (movemask)
   * (_,m,_) => ...110... (checkmask1)
   * (_,_,m) => ...011... (checkmask2)
   */
  struct PEGSOLITAIRE_EXPORT Masks {
    boost::dynamic_bitset<> moveMask, checkMask1, checkMask2;
    Masks(int size);
  };

  class PEGSOLITAIRE_EXPORT Board {
  private:
    Matrix<bool> x;
  };

}
