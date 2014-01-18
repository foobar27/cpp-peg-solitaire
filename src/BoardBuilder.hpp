#pragma once

#include "Board.hpp"
#include <utility>

namespace pegsolitaire {

  using Coordinates = std::pair<int, int>;
  using Symmetry = std::function<Coordinates(const Coordinates & bounds, const Coordinates & p)>;

  enum class Field { BLOCKED = -1, EMPTY = 0, OCCUPIED = 1};

  std::ostream& operator<<(std::ostream& os, Field field);

  class PEGSOLITAIRE_EXPORT BoardBuilder {
  public:
    BoardBuilder(const std::vector<MoveDirection> & moveDirections, const Matrix<bool> & fields);
    BoardBuilder(const std::vector<MoveDirection> & moveDirections, const Matrix<bool> && fields);

    CompactBoard encode(const Matrix<Field> &);
    Matrix<Field> decode(const CompactBoard & input);

    int population() { return m_population; }

    bool isTransformationValid(const Symmetry &) const;

  private:

    std::vector<MoveDirection> m_moveDirections;
    int m_population;
    Matrix<bool> m_fields;
    Matrix<int> m_lookupTable;
    std::vector<Masks> m_moveMasks;
  };

}
