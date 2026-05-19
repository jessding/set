#include "game.h"
#include "cli.h"

using namespace setgame;

int main() {
    Game game;
    CLI cli;
    cli.printBoard(game.board());
}