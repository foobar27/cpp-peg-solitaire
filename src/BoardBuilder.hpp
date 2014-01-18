#pragma once

#include "Board.hpp"
#include <utility>

namespace pegsolitaire {

  using Coordinates = std::pair<int, int>;
  using Symmetry = std::function<Coordinates(const Coordinates & bounds, const Coordinates & p)>;

  class PEGSOLITAIRE_EXPORT BoardBuilder {
  public:
    BoardBuilder(const std::vector<MoveDirection> & moveDirections, const Matrix<bool> & fields);
    BoardBuilder(const std::vector<MoveDirection> & moveDirections, const Matrix<bool> && fields);
  private:
    bool isTransformationValid(const Symmetry &) const;

    std::vector<MoveDirection> m_moveDirections;
    Matrix<bool> m_fields;
    Matrix<int> m_lookupTable;
    std::vector<Masks> m_moveMasks;
  };

}
