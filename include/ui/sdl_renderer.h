#pragma once

#include <SDL.h>
#include <SDL_image.h>
#include <vector>
#include "core/game.h"

namespace setgame::ui {

class SDLRenderer {
    public:
    SDLRenderer();
    ~SDLRenderer();

    void run(core::Game& game);

    private:
    static constexpr int WINDOW_WIDTH  = 1000;
    static constexpr int WINDOW_HEIGHT = 700;

    static constexpr int GRID_COLUMNS = 3;

    static constexpr int CARD_WIDTH  = 220;
    static constexpr int CARD_HEIGHT = 120;

    static constexpr int CARD_SPACING_X = 30;
    static constexpr int CARD_SPACING_Y = 20;

    static constexpr Uint32 ANIMATION_DURATION_MS = 300;

    SDL_Window* window_ = nullptr;
    SDL_Renderer* renderer_ = nullptr;

    SDL_Texture* textures_[3][3]{};

    std::vector<int> selected_;

    bool animatingRemoval_ = false;

    Uint32 animationStart_ = 0;

    void loadTextures();

    void processEvents(bool& running,
                       core::Game& game);

    void handleClick(int mouseX,
                     int mouseY,
                     core::Game& game);

    void update(core::Game& game);

    void render(const core::Game& game);

    void renderCard(const core::Card& card,
                    int index);

    SDL_Rect getCardRect(int index) const;

    bool isSelected(int index) const;

    void beginSetRemoval();

    void finishSetRemoval(core::Game& game);
};

} // namespace setgame::ui