#pragma once

#include "PegSolitaireConfig.hpp"
#include "BoardBuilder.hpp"
#include <functional>
#include <cassert>

namespace pegsolitaire {

  PEGSOLITAIRE_TEST_EXPORT Matrix<int> buildLookupTable(const Matrix<bool> & fields);

  PEGSOLITAIRE_TEST_EXPORT Coordinates transpose(const Coordinates & bounds, const Coordinates & p);
  PEGSOLITAIRE_TEST_EXPORT Coordinates vflip(const Coordinates & bounds, const Coordinates & p);
  extern const Symmetry hflip;
  extern const Symmetry rotate90;
  extern const Symmetry rotate180;
  extern const Symmetry rotate270;
  extern const Symmetry vflip_rotate90;
  extern const Symmetry hflip_rotate90;

  extern const std::vector<Symmetry> allSymmetries;

  template<typename T>

  Matrix<T> transform(const Matrix<T> & input, std::function<Coordinates(const Coordinates &, const Coordinates &)> f, const T & defaultValue) {
    int inRows = input.numberOfRows();
    int inCols = input.numberOfColumns();
    auto iBounds = Coordinates(inRows - 1, inCols - 1);
    int onRows = 0;
    int onCols = 0;
    for (int r = 0; r < inRows; ++r)
      for (int c = 0; c < inCols; ++c) {
        auto p = f(iBounds, Coordinates(r, c));
        onRows = std::max(onRows, p.first);
        onCols = std::max(onCols, p.second);
      }
    onRows++;
    onCols++;

    Matrix<T> output(onRows, onCols, defaultValue);
    for (int r = 0; r < inRows; ++r)
      for (int c = 0; c < inCols; ++c)
        output(f(iBounds, Coordinates(r, c))) = input(r, c);
    return output;
  }

  PEGSOLITAIRE_TEST_EXPORT bool haveEqualShape(const Matrix<int> & m1, const Matrix<int> & m2);

  PEGSOLITAIRE_TEST_EXPORT std::vector<Masks> buildMoveMasks(const Matrix<bool> & board, const Matrix<int> & lookupTable, const std::vector<MoveDirection> & moveDirections);

}
