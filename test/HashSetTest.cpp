#define BOOST_TEST_MODULE "HashSetTest"
#include "TestCommon.hpp"

#include <boost/random.hpp>
#include <boost/test/test_case_template.hpp>
#include <boost/mpl/list.hpp>

#include <set>
#include <vector>
#include <sstream>
#include <cmath>

#include "HashSet.hpp"

using namespace std;
using namespace pegsolitaire;

template<typename valueType, typename indexType>
struct ModuloHashFunction {
  indexType operator()(indexType capacity, valueType value) const {
    indexType h = value;
    return abs(h % capacity);
  }
};

BOOST_AUTO_TEST_CASE(hashSet_simple_int32) {
  HashSet< HashSetTraits<32> > set;
  BOOST_CHECK_EQUAL(set.size(), 0);
  set += 42;
  BOOST_CHECK(set[42]);
  BOOST_CHECK_EQUAL(set.size(), 1);
  set += 42;
  BOOST_CHECK(set[42]);
  BOOST_CHECK_EQUAL(set.size(), 1);
}

BOOST_AUTO_TEST_CASE(resizes) {
  HashSet< HashSetTraits<32> > set;
  BOOST_CHECK_EQUAL(set.capacity(), 1<<5);
  for (int i=1; i<0.74*256; ++i)
    set += i;
  // TODO do some tests
}

typedef boost::mpl::list<int,long,unsigned char> test_types;

typedef boost::mpl::list<int,long,unsigned char> test_types;

BOOST_AUTO_TEST_CASE(hashSet_int8_full_random_symmetric) {
  boost::random::mt19937 rng;
  boost::random::uniform_int_distribution<> dist(0,255);

  HashSet< HashSetTraits<8> > set;

  vector<int> requests;
  for (int i=0; i<0.70*255; ++i) {
    auto v = dist(rng);
    if (v != 0)
      requests.push_back(v);
  }

  std::set<int> seen;
  for (int r : requests) {
    set += r;
    seen.insert(r);
  }
  for (int i=1; i<256; ++i)
    BOOST_CHECK_EQUAL(set[i], seen.count(i));
}

BOOST_AUTO_TEST_CASE(emptyHashSet_stream) {
  HashSet<HashSetTraits<8, 16, ModuloHashFunction<unsigned char, unsigned short>>> set {};
  stringstream ss;
  ss << set;
  BOOST_CHECK_EQUAL(ss.str(), "{}");
}

BOOST_AUTO_TEST_CASE(singletonHashSet_stream) {
  HashSet<HashSetTraits<8, 16, ModuloHashFunction<unsigned char, unsigned short>>> set {2};
  stringstream ss;
  ss << set;
  BOOST_CHECK_EQUAL(ss.str(), "{2}");
}

BOOST_AUTO_TEST_CASE(hashSet_stream) {
  HashSet<HashSetTraits<8, 16, ModuloHashFunction<unsigned char, unsigned short>>> set {1, 2, 6, 9};
  BOOST_CHECK(set[1]);
  BOOST_CHECK(set[2]);
  BOOST_CHECK(!set[3]);
  stringstream ss;
  ss << set;
  BOOST_CHECK_EQUAL(ss.str(), "{1, 2, 6, 9}");
}
