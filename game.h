#pragma once

#include "board.h"
#include <vector>

namespace setgame {

class Game {
    public:
    Game();
    
    private:
    Board board_;
    std::vector<Card> deck_;

    void setupBoard();
    void initializeDeck();
    void shuffleDeck();
    void dealCard();
    bool hasNoSet() const;
};

} // namespace setgame