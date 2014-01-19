#define BOOST_TEST_MODULE "PegSolitaireTest"
#include "TestCommon.hpp"

#include "BoardIO.hpp"
#include "impl/BoardBuilderImpl.hpp"

#include <sstream>
#include <string>
#include <vector>
#include <utility>
#include <algorithm>

using namespace std;
using namespace pegsolitaire;

BOOST_AUTO_TEST_CASE(readLinesTest) {
  stringstream ss;
  ss << "Hello World" << endl
     << "foo" << endl
     << "bar" << endl
     << endl
     << "baz" << endl;
  auto lines = readLinesUntilBlank(ss);
}

vector<MoveDirection> allDirections = {MoveDirection::VERTICAL,
                                       MoveDirection::HORIZONTAL,
                                       MoveDirection::LEFT_DIAGONAL,
                                       MoveDirection::RIGHT_DIAGONAL};

// FIXME sorry about the strange position of the commas below, I'm fighting an emacs indent bug :(
Matrix<bool> asymmetricField {vector<vector<bool>> {
    {false,  false, true,  false, false}
    ,{false, true,  true,  true,  false}
    ,{true,  true,  true,  true,  true}
    ,{false, true,  false, true,  false}
    ,{false, false, true,  false, false}}};

BOOST_AUTO_TEST_CASE(parseLinesTest) {
  vector<string> lines = {
    "..o..",
    ".ooo.",
    "ooooo",
    "   .o.o.",
    "..o..",
  };
  auto expected = asymmetricField;
  Matrix<bool> m = parseLines(lines);
  BOOST_CHECK_EQUAL(m, expected);
}

Matrix<int> intMatrix(initializer_list<initializer_list<int>> input) {
  vector<vector<int>> data;
  for (auto i : input) {
    data.push_back(vector<int>(i));
  }
  return Matrix<int>(data);
}

void verifySymmetry(const Matrix<int> & m, const Symmetry & f, initializer_list<initializer_list<int>> expected) {
  BOOST_CHECK_EQUAL(transform(m, f, 0), intMatrix(expected));
}

BOOST_AUTO_TEST_CASE(symmetriesTest) {
  Matrix<int> m1 = intMatrix({{1,2,3}, {4,5,6}, {7,8,9}});
  verifySymmetry(m1, transpose, {{1,4,7}, {2,5,8}, {3,6,9}});
  verifySymmetry(m1, vflip,     {{3,2,1}, {6,5,4}, {9,8,7}});
  verifySymmetry(m1, hflip,     {{7,8,9}, {4,5,6}, {1,2,3}});
  verifySymmetry(m1, rotate90,  {{7,4,1}, {8,5,2}, {9,6,3}});
  verifySymmetry(m1, rotate180, {{9,8,7}, {6,5,4}, {3,2,1}});
  verifySymmetry(m1, rotate270, {{3,6,9}, {2,5,8}, {1,4,7}});
  verifySymmetry(m1, vflip_rotate90, {{1,4,7}, {2,5,8}, {3,6,9}});
  verifySymmetry(m1, hflip_rotate90, {{9,6,3}, {8,5,2}, {7,4,1}});

  // matrix is not a square
  Matrix<int> m2 = intMatrix({{1,2,3}, {4,5,6}});
  verifySymmetry(m2, transpose, {{1, 4}, {2, 5}, {3, 6}});
}

BOOST_AUTO_TEST_CASE(lookupTableTest) {
  BOOST_CHECK_EQUAL(buildLookupTable(asymmetricField),
                    intMatrix({
                        { -1, -1, 11, -1, -1}
                        ,{-1, 10,  9,  8, -1}
                        ,{ 7,  6,  5,  4,  3}
                        ,{-1,  2, -1,  1, -1}
                        ,{-1, -1,  0, -1, -1}}));
}

BOOST_AUTO_TEST_CASE(moveMasksTest) {
  auto masks = buildMoveMasks(asymmetricField, buildLookupTable(asymmetricField), allDirections);
  // TODO
}

BOOST_AUTO_TEST_CASE(BoardBuilder_test) {
  BoardBuilder builder(allDirections, asymmetricField);

  // decode some known value of a board
  Matrix<Field> expected(vector<vector<Field>> {
      { Field::BLOCKED,  Field::BLOCKED,  Field::EMPTY,    Field::BLOCKED,  Field::BLOCKED}
      ,{Field::BLOCKED,  Field::EMPTY,    Field::EMPTY,    Field::EMPTY,    Field::BLOCKED}
      ,{Field::EMPTY,    Field::EMPTY,    Field::EMPTY,    Field::EMPTY,    Field::EMPTY  }
      ,{Field::BLOCKED,  Field::OCCUPIED, Field::BLOCKED,  Field::OCCUPIED, Field::BLOCKED}
      ,{Field::BLOCKED,  Field::BLOCKED,  Field::EMPTY,    Field::BLOCKED,  Field::BLOCKED}
    });
  BOOST_CHECK_EQUAL(builder.decode(CompactBoard(builder.population(), 6)),
                    expected);

  // decode/encode-cycle for every possible field
  int numberOfPossibleFields = 1 << builder.population();
  for (int i = 0; i < numberOfPossibleFields; ++i) {
    CompactBoard board(builder.population(), i);
    BOOST_CHECK_EQUAL(board, builder.encode(builder.decode(board)));
  }

  int n = count_if(allSymmetries.begin(),
                   allSymmetries.end(),
                   bind(&BoardBuilder::isTransformationValid, &builder, std::placeholders::_1));
  BOOST_CHECK_EQUAL(n, 1 ); // only vflip

  // TODO further consistency tests
}
