#pragma once

#include "Board.hpp"
#include "AST.hpp"
#include <map>
#include <utility>

namespace pegsolitaire {

using Coordinates = std::pair<int, int>;
using Symmetry = std::function<Coordinates(const Coordinates & bounds, const Coordinates & p)>;

enum class Field { BLOCKED = -1, EMPTY = 0, OCCUPIED = 1};

std::ostream& operator<<(std::ostream& os, Field field);

class PEGSOLITAIRE_EXPORT BoardCompiler {
public:
    BoardCompiler(const std::vector<MoveDirection> & moveDirections, const Matrix<bool> & fields);
    BoardCompiler(const std::vector<MoveDirection> & moveDirections, const Matrix<bool> && fields) = delete;

    CompactBoard encode(const Matrix<Field> &) const;
    Matrix<Field> decode(const CompactBoard & input) const;

    int population() const { return m_population; }

    bool isTransformationValid(const Symmetry &) const;
    ast::Function<unsigned long, unsigned long> generateNormalForm() const;
    //ast::Function<unsigned long, unsigned long> generateEquivalentFields(const Procedure<CompactBoard> & callback) const;

private:

    std::vector<int> computePermutations(const Matrix<int> & lookupTable) const;
    // TODO rename to something useful: moveOperations, checkOperations?
    std::map<int, CompactBoard> calculateOperations(const Matrix<int> & lookupTable) const;
    ast::Function<unsigned long, unsigned long> generateSymmetryFunction(const Matrix<int> & lookupTable) const;

    std::vector<MoveDirection> m_moveDirections;
    int m_population;
    Matrix<bool> m_fields;
    Matrix<int> m_lookupTable;
    std::vector<Masks> m_masks;
    std::vector<ast::Function<unsigned long, unsigned long>> m_symmetryFunctions;
};

}
