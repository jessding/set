#include "core/game.h"
#include "ui/sdl_renderer.h"

#include <iostream>

using namespace setgame::core;

int main() {
    try {
        setgame::core::Game game;
        setgame::ui::SDLRenderer renderer;
        renderer.run(game);
    } catch (const std::exception &e) {
        std::cerr << "Fatal error: " << e.what() << "\n";
        return 1;
    }
    return 0;
}