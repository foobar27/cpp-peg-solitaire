#define BOOST_TEST_MAIN
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE "pegsolitaire_test"

#include <boost/test/unit_test.hpp>

#include "BoardIO.hpp"
#include "impl/BoardBuilderImpl.hpp"

#include <sstream>
#include <string>
#include <vector>
#include <utility>

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
    "oo.oo",
    "o...o",
    ".....",
    "   o.o.o",
    "oo.oo",
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
  BOOST_CHECK_EQUAL(transform(m, f), intMatrix(expected));
}

BOOST_AUTO_TEST_CASE(symmetriesTest) {
  Matrix<int> m(vector<vector<int>>({{1,2,3}, {4,5,6}, {7,8,9}}));
  verifySymmetry(m, transpose, {{1,4,7}, {2,5,8}, {3,6,9}});
  verifySymmetry(m, vflip,     {{3,2,1}, {6,5,4}, {9,8,7}});
  verifySymmetry(m, hflip,     {{7,8,9}, {4,5,6}, {1,2,3}});
  verifySymmetry(m, rotate90,  {{7,4,1}, {8,5,2}, {9,6,3}});
  verifySymmetry(m, rotate180, {{9,8,7}, {6,5,4}, {3,2,1}});
  verifySymmetry(m, rotate270, {{3,6,9}, {2,5,8}, {1,4,7}});
  verifySymmetry(m, vflip_rotate90, {{1,4,7}, {2,5,8}, {3,6,9}});
  verifySymmetry(m, hflip_rotate90, {{9,6,3}, {8,5,2}, {7,4,1}});
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
  // TODO perform tests!!!
}
