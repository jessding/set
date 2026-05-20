#pragma once

#include "core/board.h"
#include <vector>

namespace setgame::core {

class Game {
    public:
    Game();
    const Board& board() const { return board_; }
    void removeSelectedCards(const std::vector<int>& indices);
    void refillBoard();

    private:
    Board board_;
    std::vector<Card> deck_;

    void initializeDeck();
    void shuffleDeck();
    void dealCard();
    bool hasNoSet() const;
};

} // namespace setgame::core