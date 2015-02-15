#include "impl/BoardCompilerImpl.hpp"

#include <initializer_list>
#include <sstream>

namespace pegsolitaire {

using namespace std;
using namespace ast;

ostream& operator<<(ostream& os, Field field) {
    switch (field) {
    case Field::OCCUPIED: os << "OCCUPIED";
        break;
    case Field::EMPTY: os << "EMPTY";
        break;
    case Field::BLOCKED: os << "BLOCKED";
        break;
    }
    return os;
}


int populationCount(const CompactBoard & v) {
    int count = 0;
    for (size_t i=0; i<v.size(); ++i) {
        if (v[i])
            ++count;
    }
    return count;
}

int populationCount(const vector<bool> & v) {
    // let's hope the population count is optimized?
    return count(v.begin(), v.end(), true);
}

int populationCount(const Matrix<bool> & m) {
    // let's hope the population count is optimized?
    return count(m.data().begin(), m.data().end(), true);
}

// TODO test
Matrix<int> buildLookupTable(const Matrix<bool> & fields) {
    const int nRows = fields.numberOfRows();
    const int nCols = fields.numberOfColumns();
    Matrix<int> result(nRows, nCols, -1);
    auto fieldIt = fields.data().begin();
    auto resultIt = result.data().begin();
    int pos = populationCount(fields) - 1;
    for (; fieldIt != fields.data().end(); ++fieldIt, ++resultIt)
        if (*fieldIt)
            *resultIt = pos--;
    assert(pos == -1);
    return result;
}

vector<Coordinates> applyMove(const Coordinates & c, MoveDirection moveDirection) {
    switch (moveDirection) {
    case MoveDirection::HORIZONTAL:
        return {Coordinates(c.first, c.second + 1),
                    Coordinates(c.first, c.second - 1)};
    case MoveDirection::VERTICAL:
        return {Coordinates(c.first + 1, c.second),
                    Coordinates(c.first - 1, c.second)};
    case MoveDirection::LEFT_DIAGONAL: // '\'
        return {Coordinates(c.first + 1, c.second + 1),
                    Coordinates(c.first - 1, c.second - 1)};
    case MoveDirection::RIGHT_DIAGONAL: // '/'
        return {Coordinates(c.first - 1, c.second + 1),
                    Coordinates(c.first + 1, c.second - 1)};
    default:
        throw runtime_error("unknown moveDirection!");
    }
}

Coordinates applyCanonicalMove(const Coordinates & c, MoveDirection moveDirection) {
    return applyMove(c, moveDirection)[0];
}

Symmetry compose(const initializer_list<Symmetry> & _fs) {
    assert(_fs.size() != 0);
    vector<Symmetry> fs = _fs;
    reverse(fs.begin(), fs.end());
    auto it = fs.begin();
    auto result = *it;
    ++it;
    for (; it != fs.end(); ++it) {
        auto f = *it;
        result = [f, result](const Coordinates & bounds, const Coordinates & p) {
            // remember: the 'fs' is reversed
            return f(bounds, result(bounds, p));
        };
    }
    return result;
}

Coordinates transpose(const Coordinates & bounds, const Coordinates & p) {
    return Coordinates(p.second, p.first);
}

Coordinates vflip(const Coordinates & bounds, const Coordinates & p) {
    return Coordinates(p.first, bounds.second - p.second);
}

const Symmetry hflip = compose({transpose, vflip, transpose});
const Symmetry rotate90 = compose({vflip, transpose});
const Symmetry rotate180 = compose({vflip, hflip});
const Symmetry rotate270 = compose({hflip, transpose});
const Symmetry vflip_rotate90 = compose({vflip, rotate90});
const Symmetry hflip_rotate90 = compose({hflip, rotate90});

const vector<Symmetry> allSymmetries =
{
    vflip,
    hflip,
    rotate90,
    rotate180,
    rotate270,
    vflip_rotate90,
    hflip_rotate90,
};

bool haveEqualShape(const Matrix<int> & m1, const Matrix<int> & m2) {
    int nRows = m1.numberOfRows();
    int nCols = m1.numberOfColumns();
    if (m2.numberOfRows() != nRows || m2.numberOfColumns() != nCols)
        return false;
    auto it1 = m1.data().begin();
    auto it2 = m2.data().begin();
    for (; it1 != m1.data().end(); ++it1, ++it2)
        if ((*it1 == -1 || *it2 == -1) && *it1 != *it2)
            return false;
    return true;
}

CompactBoard BoardCompiler::encode(const Matrix<Field> & input) const {
    int nRows = m_lookupTable.numberOfRows();
    int nCols = m_lookupTable.numberOfColumns();
    CompactBoard result(m_population, 0);
    auto lookupIt = m_lookupTable.data().begin();
    auto inputIt = input.data().begin();
    for (int r = 0; r < nRows; ++r)
        for (int c = 0; c < nCols; ++c) {
            if (*lookupIt >= 0)
                result[*lookupIt] = (*inputIt == Field::OCCUPIED);
            ++lookupIt;
            ++inputIt;
        }
    return result;
}

Matrix<Field> BoardCompiler::decode(const CompactBoard & input) const {
    int nRows = m_lookupTable.numberOfRows();
    int nCols = m_lookupTable.numberOfColumns();
    Matrix<Field> result(m_lookupTable.numberOfRows(),
                         m_lookupTable.numberOfColumns(),
                         Field::BLOCKED);
    auto lookupIt = m_lookupTable.data().begin();
    auto resultIt = result.data().begin();
    for (int r = 0; r < nRows; ++r)
        for (int c = 0; c < nCols; ++c) {
            int v = *lookupIt;
            if (v >= 0) // default value for result is Field::BLOCKED
                *resultIt = input[v] ? Field::OCCUPIED : Field::EMPTY;
            ++lookupIt;
            ++resultIt;
        }
    return result;
}

bool BoardCompiler::isTransformationValid(const Symmetry & s) const {
    if (!haveEqualShape(m_lookupTable, transform(m_lookupTable, s, 0)))
        return false;
    for (const auto & m : m_masks) {
        auto transformed = encode(transform(decode(m.moveMask), s, Field::OCCUPIED));
        bool found = false;
        for (const auto & x : m_masks)
            if (x.moveMask == transformed) {
                found = true;
                break;
            }
        if (!found)
            return false; // symmetry broken
    }
    return true;
}

vector<Masks> buildMoveMasks(const Matrix<bool> & board, const Matrix<int> & lookupTable, const vector<MoveDirection> & moveDirections) {
    const int population = populationCount(board);
    std::vector<Masks> result;
    int nRows = board.numberOfRows();
    int nCols = board.numberOfColumns();

    const auto addMove = [&lookupTable, population, & result] (const Coordinates & p1,
            const Coordinates & p2,
            const Coordinates & p3) {
        Masks masks(population);
        auto l1 = lookupTable(p1);
        auto l2 = lookupTable(p2);
        auto l3 = lookupTable(p3);
        assert(0 <= l1);
        assert(l1 < population);
        assert(0 <= l2);
        assert(l2 < population);
        assert(0 <= l3);
        assert(l3 < population);

        masks.moveMask[l1] = true;
        masks.moveMask[l2] = true;
        masks.moveMask[l3] = true;

        masks.checkMask1[l1] = true;
        masks.checkMask1[l2] = true;

        masks.checkMask2[l2] = true;
        masks.checkMask2[l3] = true;
        result.push_back(move(masks));
    };

    auto lookupTableIt = lookupTable.data().begin();

    const auto isValid = [&board, &lookupTable, nRows, nCols](const Coordinates & coord) {
        return coord.first >= 0 && coord.second >= 0
                && coord.first < nRows && coord.second < nCols
                && lookupTable(coord) >= 0;
    };

    for (int r = 0; r < nRows; ++r)
        for (int c = 0; c < nCols; ++c) {
            Coordinates coord = Coordinates(r, c);
            if (*lookupTableIt >= 0) {
                assert(isValid(coord));
                for (auto dir : moveDirections) {
                    auto n1 = applyCanonicalMove(coord, dir);
                    auto n2 = applyCanonicalMove(n1, dir);
                    if (isValid(n1) && isValid(n2))
                        addMove(coord, n1, n2);
                }
            }
            ++lookupTableIt;
        }
#ifndef NDEBUG
    for (const auto & masks : result) {
        assert(populationCount(masks.moveMask) == 3);
        assert(populationCount(masks.checkMask1) == 2);
        assert(populationCount(masks.checkMask2) == 2);
        assert(populationCount(masks.moveMask & masks.checkMask1) == 2);
        assert(populationCount(masks.moveMask & masks.checkMask2) == 2);
        assert(populationCount(masks.moveMask | masks.checkMask1) == 3);
        assert(populationCount(masks.moveMask | masks.checkMask2) == 3);
    }
#endif
    return result;
}

BoardCompiler::BoardCompiler(const vector<MoveDirection> & moveDirections, const Matrix<bool> & fields)
    : m_module(new llvm::Module("pegsolitaire jit", llvm::getGlobalContext()))
    , m_codegen(m_module)
    , m_moveDirections(moveDirections)
    , m_population(populationCount(fields))
    , m_fields(fields)
    , m_lookupTable(buildLookupTable(m_fields))
    , m_masks(buildMoveMasks(fields, m_lookupTable, moveDirections))
{
    int i = 0;
    for (auto & s : allSymmetries) {
        if (isTransformationValid(s)) {
            stringstream fName;
            fName << "applySymmetry" << i; // TODO encode real name of symmetry
            Variable arg("arg");
            auto f = m_codegen.generateFunction<uint64_t(uint64_t)>(fName.str(), {arg}, generateCode(transform(m_lookupTable, s, 0), arg));
            f->dump();
            m_symmetryFunctions.push_back(f);
        }
        ++i;
    }
}

vector<int> BoardCompiler::computePermutations(const Matrix<int> & lookupTable) const {
    auto it = lookupTable.data().begin();
    vector<int> permutations(m_population, 0);
    for (int i = m_population - 1; i >= 0; --i) {
        while (*it == -1)
            ++it;
        permutations[i] =*it;
        ++it;
    }
    return permutations;
}

map<int, CompactBoard> BoardCompiler::calculateOperations(const Matrix<int> & lookupTable) const {
    map<int, CompactBoard> output;
    auto perm = computePermutations(lookupTable);
    for (int i = 0; i < m_population; ++i) {
        CompactBoard mask(m_population, 0);
        mask[i] = true;
        int diff = perm[i] - i;
        if (output.find(diff) == output.end())
            output[diff] = mask;
        else
            output[diff] |= mask;
    }
    return output;
}

Expression BoardCompiler::generateCode(const Matrix<int> & lookupTable, const Variable & f) const {
    auto operations = calculateOperations(lookupTable);
    Expression x = boost::dynamic_bitset<>(); // TODO provide some kind of default initializer?
    for (auto & p : operations) {
        auto diff = p.first;
        auto mask = p.second;
        x = x | (f & mask) << diff; // TODO define operator |=
    }
    return x;
}

// Expression BoardCompiler::generateNormalForm(const Variable & v) const {
//   auto n = v; // identity transformation
//   for (auto & f : m_symmetryFunctions)
//     n = min(n, f(v));
//   return n;
// }

// Expression BoardCompiler::generateEquivalentFields(const Variable & f, const Procedure<CompactBoard> & callback) const {
//   Expression result;
//   for (auto & f : m_symmetryFunctions)
//     result ,= callback(f(v));
//   return result;
// }

}
