#include "core/board.h"
#include <algorithm>

namespace setgame::core {

void Board::add(const Card& c) {
    cards_.push_back(c);
}

bool Board::contains(const Card& c) const {
    return std::find(cards_.begin(), cards_.end(), c) != cards_.end();
}

std::vector<Set> Board::findAllSets() const {
    // rudimentary O(n^3) impl for now
    std::vector<Set> result;
    int boardSize = cards_.size();

    for (int i = 0; i < boardSize; i++) {
        for (int j = i+1; j < boardSize; j++) {
            const Card& a = cards_[i], b = cards_[j];

            Card c = getThirdCard(a, b);

            if (contains(c)) result.push_back({a, b, c});
        }
    }
    return result;
}

void Board::removeIndices(const std::vector<int>& indices) {
    std::vector<int> sorted = indices;
    std::sort(sorted.rbegin(), sorted.rend());
    for (int index : sorted) {
        cards_.erase(cards_.begin() + index);
    }
}

} // namespace setgame::core