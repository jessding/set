#pragma once

#include <vector>
#include "core/rules.h"

namespace setgame::core {
    class Board {
    public:
        void add(const Card &c);

        void replace(int index, const Card &c);

        bool contains(const Card &c) const;

        std::vector<Set> findAllSets() const;

        const std::vector<Card> &cards() const { return cards_; }

        void removeIndices(const std::vector<int> &indices);

    private:
        std::vector<Card> cards_;
    };
} // namespace setgame::core
