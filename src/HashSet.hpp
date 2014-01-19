#pragma once

#include "PegSolitaireConfig.hpp"
#include <boost/integer.hpp>
#include <vector>
#include <utility>
#include <cstring>
#include <boost/iterator/iterator_facade.hpp>
#include <boost/iterator/filter_iterator.hpp>

namespace pegsolitaire {

  template<unsigned int VALUE_BIT_WIDTH, unsigned int INDEX_BIT_WIDTH = 2*VALUE_BIT_WIDTH>
  struct HashSetTraits {
    static constexpr int minCapacity = 1 << 5;
    using valueType = typename boost::uint_t<VALUE_BIT_WIDTH>::least;
    using indexType = typename boost::uint_t<INDEX_BIT_WIDTH>::least; // TODO this is a quick hack.
  };

  template <class hashSetT>
  class HashSet;

  namespace {
    template<class hashSetT>
    struct IsValidElement {
      bool operator()(int x) {
        return x != HashSet<hashSetT>::invalidElement;
      }
    };

    template <class Value>
    class data_iter : public boost::iterator_adaptor<
      data_iter<Value>,
      Value*,
      Value,
      boost::forward_traversal_tag,
      Value // don't use references (Value is usually a primitive type)
      >
    {
    private:
      struct enabler {};  // a private type avoids misuse
      friend class boost::iterator_core_access;
      void increment() {
        ++this->base_reference();

      }

    public:
      data_iter()
        : data_iter::iterator_adaptor_(0) {}

      explicit data_iter(Value* p)
        : data_iter::iterator_adaptor_(p) {}

      template <class OtherValue>
      data_iter(
                data_iter<OtherValue> const& other
                , typename boost::enable_if<
                boost::is_convertible<OtherValue*,Value*>
                , enabler
                >::type = enabler()
                )
        : data_iter::iterator_adaptor_(other.base()) {}
    };

  }

  template<class hashSetT>
  class HashSet {
  public:
    using valueType = typename hashSetT::valueType;
    using indexType = typename hashSetT::indexType;
    using const_iterator = boost::filter_iterator<
      IsValidElement<hashSetT>,
      data_iter<const valueType>>;
    static constexpr valueType invalidElement = 0;
    static constexpr indexType minCapacity = hashSetT::minCapacity;

    HashSet() : HashSet(minCapacity) {}

    HashSet(std::initializer_list<valueType> values)
      : HashSet() // this is not quite efficient, but this ctor is used for tests only
    {
      ensureCapacityFor(values.size());
      for (auto v : values)
        *this += v;
    }

    indexType size() const {
      return m_size;
    }

    indexType capacity() const {
      return m_capacity;
    }

    bool operator[](valueType value) const {
      return m_table[findOrEmpty(value)] != invalidElement;
    }

    bool operator+=(valueType value) {
      ensureCapacityFor(m_size+1);
      return internalAdd(value);
    }

    const_iterator begin() const {
      return const_iterator(data_iter<valueType>(m_table),
                            data_iter<valueType>(m_table + m_capacity));
    }

    const_iterator end() const {
      return const_iterator(data_iter<valueType>(m_table + m_capacity),
                            data_iter<valueType>(m_table + m_capacity));
    }

  private:
    indexType m_size;
    indexType m_capacity; // TODO remove?
    valueType *m_table;

    // TODO move to trait!
    indexType getIndex(valueType value) const {
      indexType h = value;
      // Copied from Apache's AbstractHashedMap; prevents power-of-two collisions.
      h += ~(h << 9);
      h ^= (h >> 14);
      h += (h << 4);
      h ^= (h >> 10);
      // Power of two trick.
      return h & (m_capacity-1);
    }

    indexType findOrEmpty(indexType value) const {
      assert(value != invalidElement);
      indexType index = getIndex(value);
      do {
        const valueType existing = m_table[index];
        if (likely(value == existing || existing == invalidElement))
          return index;
        index++;
        index = index % m_capacity; // TODO use bit operations
      } while (true);
    }

    indexType find(valueType value) const {
      indexType index = findOrEmpty(value);
      if (m_table[index] == invalidElement)
        return -1; // TODO correct value?
      return index;
    }

    bool internalAddValidNoCount(valueType value) {
      indexType index = findOrEmpty(value);
      if (m_table[index] == invalidElement) {
        m_table[index] = value;
        return true;
      }
      return false;
    }

    bool internalAdd(valueType value) {
      assert(value != invalidElement);
      bool result = internalAddValidNoCount(value);
      if (result)
        m_size++;
      return result;
    }

    void internalAddOnlyValidNoCount(valueType* values, indexType n) {
      for (indexType i=0; i<n; ++i) {
        valueType value = values[i];
        if (value != invalidElement)
          internalAddValidNoCount(value);
      }
    }

    inline bool sizeFitsIntoCapacity(indexType expectedSize, indexType capacity) {
      // expectedSize < 0.75 * m_capacity
      return 4 * expectedSize < 3 * capacity;
    }

    indexType computeCapacityForSize(indexType expectedSize) {
      indexType newCapacity = m_capacity;
      while (!sizeFitsIntoCapacity(expectedSize, newCapacity))
        newCapacity <<= 1;
      return newCapacity;
    }

    void adjustCapacityTo(indexType newCapacity) {
      assert(newCapacity>0);
      assert(invalidElement == 0);
      m_table = new valueType[newCapacity];
      m_capacity = newCapacity;
      memset(m_table, 0, m_capacity * sizeof(valueType));
    }

    void ensureCapacityFor(indexType expectedSize) {
      // fast-path
      if (likely(sizeFitsIntoCapacity(expectedSize, m_capacity)))
        return; // nothing to do

      indexType oldCapacity = m_capacity;
      indexType newCapacity = computeCapacityForSize(expectedSize);
      if (newCapacity == oldCapacity)
        return;
      valueType* oldTable = m_table;
      adjustCapacityTo(newCapacity);
      if (oldTable) {
        internalAddOnlyValidNoCount(oldTable, oldCapacity);
        delete[] oldTable;
      }
    }

    HashSet(indexType capacity)
      : m_size(0)
      , m_capacity(capacity)
      , m_table(nullptr)
    {
      adjustCapacityTo(capacity);
    }

  };

  template<class hashSetT>
  std::ostream& operator<<(std::ostream& os, const HashSet<hashSetT> & set) {
    os << "{";
    bool first = true;
    for (auto i : set) {
      if (!first)
        os << ", ";
      os << +i;
      first = false;
    }
    os << "}";
    return os;
  }

}
