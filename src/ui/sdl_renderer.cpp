#include "ui/sdl_renderer.h"
#include <algorithm>
#include <stdexcept>

namespace setgame::ui {

namespace {

enum Shape {
    DIAMOND = 0,
    OVAL = 1,
    SQUIGGLE = 2
};

enum Shading {
    OPEN = 0,
    STRIPED = 1,
    SOLID = 2
};

SDL_Color getColor(uint8_t color) {
    switch (color) {
        case 0:
            return {255, 0, 0, 255};
        case 1:
            return {0, 180, 0, 255};
        case 2:
            return {160, 0, 160, 255};
        default:
            return {255,255,255,255};
    }
}

} // namespace

SDLRenderer::SDLRenderer() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        throw std::runtime_error(SDL_GetError());
    }
    if (!(IMG_Init(IMG_INIT_PNG))) {
        throw std::runtime_error(IMG_GetError());
    }
    window_ = SDL_CreateWindow(
        "SET",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        0
    );
    if (!window_) throw std::runtime_error(SDL_GetError());
    loadTextures();

    renderer_ = SDL_CreateRenderer(
        window_,
        -1,
        SDL_RENDERER_ACCELERATED
    );
}

SDLRenderer::~SDLRenderer() {
    for (int s = 0; s < 3; s++) {
        for (int sh = 0; sh < 3; sh++) {

            if (textures_[s][sh]) {
                SDL_DestroyTexture(textures_[s][sh]);
            }
        }
    }

    SDL_DestroyRenderer(renderer_);
    SDL_DestroyWindow(window_);
    IMG_Quit();
    SDL_Quit();
}

void SDLRenderer::loadTextures() {
    textures_[DIAMOND][OPEN] =
        IMG_LoadTexture(renderer_,
                        "assets/diamond_open.png");

    textures_[DIAMOND][STRIPED] =
        IMG_LoadTexture(renderer_,
                        "assets/diamond_striped.png");

    textures_[DIAMOND][SOLID] =
        IMG_LoadTexture(renderer_,
                        "assets/diamond_solid.png");

    textures_[OVAL][OPEN] =
        IMG_LoadTexture(renderer_,
                        "assets/oval_open.png");

    textures_[OVAL][STRIPED] =
        IMG_LoadTexture(renderer_,
                        "assets/oval_striped.png");

    textures_[OVAL][SOLID] =
        IMG_LoadTexture(renderer_,
                        "assets/oval_solid.png");

    textures_[SQUIGGLE][OPEN] =
        IMG_LoadTexture(renderer_,
                        "assets/squiggle_open.png");

    textures_[SQUIGGLE][STRIPED] =
        IMG_LoadTexture(renderer_,
                        "assets/squiggle_striped.png");

    textures_[SQUIGGLE][SOLID] =
        IMG_LoadTexture(renderer_,
                        "assets/squiggle_solid.png");
}

void SDLRenderer::run(core::Game& game) {
    bool running = true;
    while (running) {
        processEvents(running, game);
        update(game);
        render(game);
        SDL_Delay(16);
    }
}

void SDLRenderer::processEvents(bool& running, core::Game& game) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            running = false;
        }
        if (event.type == SDL_MOUSEBUTTONDOWN &&
            !animatingRemoval_) {
            handleClick(event.button.x,
                        event.button.y,
                        game);
        }
    }
}

void SDLRenderer::handleClick(int mouseX, int mouseY, core::Game& game) {
    const auto&cards = game.board().cards();
    for (size_t i = 0; i < cards.size(); i++) {
        SDL_Rect rect = getCardRect(i);

        bool inside =
            mouseX >= rect.x &&
            mouseX <= rect.x + rect.w &&
            mouseY >= rect.y &&
            mouseY <= rect.y + rect.h;

        if (!inside) continue;
        
        auto it = std::find(selected_.begin(),
                            selected_.end(),
                            i);

        if (it != selected_.end()) {
            selected_.erase(it);
            return;
        }

        if (selected_.size() < 3) {
            selected_.push_back(i);
        }

        if (selected_.size() == 3) {
            const auto& c0 = cards[selected_[0]];
            const auto& c1 = cards[selected_[1]];
            const auto& c2 = cards[selected_[2]];
            bool valid = core::isValidSet(c0,c1,c2);
            if (valid) beginSetRemoval();
            else selected_.clear();
        }
        return;
    }
}

void SDLRenderer::beginSetRemoval() {
    animatingRemoval_ = true;
    animationStart_ = SDL_GetTicks();
}

void SDLRenderer::update(core::Game& game) {
    if (!animatingRemoval_) return;
    Uint32 elapsed = SDL_GetTicks() - animationStart_;
    if (elapsed >= ANIMATION_DURATION_MS) {
        finishSetRemoval(game);
        animatingRemoval_ = false;
    }
}

void SDLRenderer::finishSetRemoval(core::Game& game) {
    game.removeSelectedCards(selected_);
    game.refillBoard();
    selected_.clear();
} 

bool SDLRenderer::isSelected(int index) const {
    return std::find(selected_.begin(),
                     selected_.end(),
                     index)
           != selected_.end();
}

SDL_Rect SDLRenderer::getCardRect(int index) const {
    int row = index / GRID_COLUMNS;
    int col = index % GRID_COLUMNS;

    constexpr int START_X = 80;
    constexpr int START_Y = 60;

    int x =
        START_X
        + col * (CARD_WIDTH + CARD_SPACING_X);

    int y =
        START_Y
        + row * (CARD_HEIGHT + CARD_SPACING_Y);

    return {
        x,
        y,
        CARD_WIDTH,
        CARD_HEIGHT
    };
}

void SDLRenderer::render(const core::Game& game) {
    SDL_SetRenderDrawColor(renderer_,
                           30,30,30,255);
    SDL_RenderClear(renderer_);
    const auto& cards = game.board().cards();
    for (size_t i = 0; i < cards.size(); i++) {
        renderCard(cards[i], i);
    }
    SDL_RenderPresent(renderer_);
}

void SDLRenderer::renderCard(const core::Card& card,
                             int index) {

    SDL_Rect rect = getCardRect(index);
    bool selected = isSelected(index);

    if (selected) {
        SDL_SetRenderDrawColor(renderer_, 100,180,255,255);
    } else {
        SDL_SetRenderDrawColor(renderer_, 255,255,255,255);
    }

    SDL_RenderFillRect(renderer_, &rect);
    SDL_SetRenderDrawColor(renderer_,
                           0,0,0,255);
    SDL_RenderDrawRect(renderer_, &rect);

    int number  = card[0];
    int color   = card[1];
    int shading = card[2];
    int shape   = card[3];

    SDL_Texture* texture = textures_[shape][shading];

    SDL_Color tint = getColor(color);

    SDL_SetTextureColorMod(texture,
                           tint.r,
                           tint.g,
                           tint.b);

    int count = number + 1;

    int symbolWidth  = 50;
    int symbolHeight = 30;
    int spacing = 15;

    int totalWidth =
        count * symbolWidth
        + (count - 1) * spacing;

    int startX =
        rect.x + rect.w / 2
        - totalWidth / 2;

    for (int i = 0; i < count; i++) {
        int symbolX =
            startX
            + i * (symbolWidth + spacing);

        int symbolY =
            rect.y + rect.h / 2
            - symbolHeight / 2;

        SDL_Rect dst{
            symbolX,
            symbolY,
            symbolWidth,
            symbolHeight
        };

        if (animatingRemoval_ && isSelected(index)) {
            Uint32 elapsed =
                SDL_GetTicks() - animationStart_;

            float t =
                1.0f -
                (float(elapsed)
                / ANIMATION_DURATION_MS);

            dst.w *= t;
            dst.h *= t;

            dst.x += (symbolWidth - dst.w) / 2;
            dst.y += (symbolHeight - dst.h) / 2;
        }

        SDL_RenderCopy(renderer_,
                       texture,
                       nullptr,
                       &dst);
    }
}

} // namespace setgame::ui