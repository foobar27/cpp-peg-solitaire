#pragma once

#include <cassert>
#include <algorithm>
#include <vector>
#include <utility>
#include <iostream>

namespace pegsolitaire {

template<typename T>
class Matrix {
public:
    typedef typename std::vector<T>::value_type value_type;
    typedef typename std::vector<T>::reference reference;
    typedef typename std::vector<T>::const_reference const_reference;

    Matrix(int numberOfRows, int numberOfColumns, const T & defaultValue)
        : m_numberOfRows(numberOfRows)
        , m_numberOfColumns(numberOfColumns)
        , m_data(numberOfRows * numberOfColumns, defaultValue)
    {}

    Matrix(const std::vector<std::vector<T>> & rows) {
        assert(rows.size() > 0 && rows[0].size() > 0);
        m_numberOfRows = rows.size();
        m_numberOfColumns = rows[0].size();
        m_data = std::vector<T>(numberOfRows() * numberOfColumns());
        auto it = m_data.begin();
        for (const auto & row : rows) {
            assert(row.size() == m_numberOfColumns);
            std::copy(row.begin(), row.end(), it);
            it += m_numberOfColumns;
        }
    }

    int numberOfRows() const {
        return m_numberOfRows;
    }

    int numberOfColumns() const {
        return m_numberOfColumns;
    }

    reference operator()(int row, int column) {
        assertIndexValid(row, column);
        return m_data[index(row, column)];
    }

    const_reference operator()(int row, int column) const {
        assertIndexValid(row, column);
        return m_data[index(row, column)];
    }

    reference operator()(const std::pair<int, int> & coord) {
        return operator()(coord.first, coord.second);
    }

    const_reference operator()(const std::pair<int, int> & coord) const {
        return operator()(coord.first, coord.second);
    }

    bool operator==(const Matrix<T> & other) const {
        return m_data == other.m_data;
    }

    std::vector<T> & data() {
        return m_data;
    }

    const std::vector<T> & data() const {
        return m_data;
    }


private:

    inline int index(int row, int column) const {
        return column + m_numberOfColumns * row;
    }

    void assertIndexValid(int row, int column) const {
        assert(0 <= row && row < m_numberOfRows);
        assert(0 <= column && column < m_numberOfColumns);
    }

    int m_numberOfRows, m_numberOfColumns;
    std::vector<T> m_data;
};

template <typename T>
std::ostream& operator<<(std::ostream& os, const Matrix<T> & m) {
    int nRows = m.numberOfRows();
    int nCols = m.numberOfColumns();
    os << "{";
    auto it = m.data().begin();
    for (int r=0; r<nRows; ++r) {
        if (r != 0)
            os << ",";
        os << "{";
        for(int c=0; c<nCols; ++c) {
            if (c != 0)
                os << ",";
            os << *it;
            it++;
        }
        os << "}";
    }
    os << "}";
    return os;
}

}
