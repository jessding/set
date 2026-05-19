#include "cli.h"

#include <iostream>
#include <iomanip>

namespace setgame {

namespace {
    void printCard(const Card& c) {
        std::cout
            << "("
            << int(c[0]) << " "
            << int(c[1]) << " "
            << int(c[2]) << " "
            << int(c[3]) << ")";
    }
}

void CLI::printBoard(const Board& board) const {
    int columns = 3;
    const auto& cards = board.cards();
    for (int i = 0; i < cards.size(); i++) {
        std::cout << std::setw(2) << i << ": ";
        printCard(cards[i]);
        std::cout << "    ";
        if ((i+1) % columns == 0) {
            std::cout << "\n";
        }
    }
}

} //namespace setgame