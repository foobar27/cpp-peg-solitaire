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

  // FIXME remove defaultValue if possible
  Matrix<T> transform(const Matrix<T> & input, std::function<Coordinates(const Coordinates &, const Coordinates &)> f, const T & defaultValue) {
    int nRows = input.numberOfRows();
    int nCols = input.numberOfColumns();
    assert(nRows == nCols); // currently only square matrices supported
    const auto bounds = Coordinates(nRows - 1, nCols - 1);
    auto transformedBounds = bounds; // OK, because square matrix
    Matrix<T> output(transformedBounds.first + 1, transformedBounds.second + 1, defaultValue);
    for (int r = 0; r < nRows; ++r)
      for (int c = 0; c < nCols; ++c)
        output(f(bounds, Coordinates(r, c))) = input(r, c);
    return output;
  }

  PEGSOLITAIRE_TEST_EXPORT bool haveEqualShape(const Matrix<int> & m1, const Matrix<int> & m2);

  PEGSOLITAIRE_TEST_EXPORT std::vector<Masks> buildMoveMasks(const Matrix<bool> & board, const Matrix<int> & lookupTable, const std::vector<MoveDirection> & moveDirections);

}
