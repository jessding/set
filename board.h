#pragma once

#include <vector>
#include "rules.h"

namespace setgame {

class Board {
    public:
    void add(const Card& c);
    bool contains(const Card& c) const;
    std::vector<Set> findAllSets() const;
    const std::vector<Card>& cards() const { return cards_; }

    private:
    std::vector<Card> cards_;
};

} // namespace setgame