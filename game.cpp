#include "game.h"

#include <algorithm>
#include <random>

namespace setgame {

    Game::Game() {
        initializeDeck();
        shuffleDeck();
        setupBoard();
    };

    void Game::initializeDeck() {
        deck_.clear();
        for (uint8_t t1 = 0; t1 < 3; t1++) {
            for (uint8_t t2 = 0; t2 < 3; t2++) {
                for (uint8_t t3 = 0; t3 < 3; t3++) {
                    for (uint8_t t4 = 0; t4 < 3; t4++) {
                        deck_.push_back({t1, t2, t3, t4});
                    }
                }
            }
        }
    }
  
    void Game::shuffleDeck() {
        std::random_device rg;
        std::mt19937 generator(rg());
        std::shuffle(deck_.begin(), deck_.end(), generator);
    }

    void Game::setupBoard() {
        for (int c = 0; c < 12; c++) {
            dealCard();
        }
        while (hasNoSet()) {
            dealCard();
        }
    }

    void Game::dealCard() {
        board_.add(deck_.back());
        deck_.pop_back();
    }

    bool Game::hasNoSet() const {
        return board_.findAllSets().empty();
    }

} // namespace setgame