#pragma once

#include "PegSolitaireConfig.hpp"
#include "Board.hpp"

#include <vector>
#include <string>
#include <istream>

namespace pegsolitaire {
PEGSOLITAIRE_EXPORT std::vector<std::string> readLinesUntilBlank(std::istream & is);
PEGSOLITAIRE_EXPORT Matrix<bool> parseLines(const std::vector<std::string> & lines);
}
