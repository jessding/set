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
                    return {255, 255, 255, 255};
            }
        }
    } // namespace

    SDLRenderer::SDLRenderer() {
        if (SDL_Init(SDL_INIT_VIDEO) != 0) throw std::runtime_error(SDL_GetError());

        if (!(IMG_Init(IMG_INIT_PNG))) throw std::runtime_error(IMG_GetError());

        window_ = SDL_CreateWindow(
            "SET",
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            WINDOW_WIDTH,
            WINDOW_HEIGHT,
            0
        );

        if (!window_) throw std::runtime_error(SDL_GetError());

        renderer_ = SDL_CreateRenderer(
            window_,
            -1,
            SDL_RENDERER_ACCELERATED
        );

        if (!renderer_) throw std::runtime_error(SDL_GetError());

        SDL_RendererInfo info;
        SDL_GetRendererInfo(renderer_, &info);

        SDL_Log("Renderer: %s", info.name);

        std::fill(
            &textures_[0][0],
            &textures_[0][0] + 9,
            nullptr
        );

        loadTextures();
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
        auto load = [&](SDL_Texture *&tex, const char *path) {
            SDL_Surface *surface = IMG_Load(path);
            if (!surface) {
                SDL_Log("Failed to load %s: %s", path, IMG_GetError());
                return;
            }
            tex = SDL_CreateTextureFromSurface(renderer_, surface);
            SDL_FreeSurface(surface);
            if (!tex) {
                SDL_Log("CreateTextureFromSurface failed: %s", SDL_GetError());
                return;
            }
            SDL_SetTextureBlendMode(tex, SDL_BLENDMODE_BLEND);
        };

        load(textures_[DIAMOND][OPEN],
             "assets/diamond_open.png");

        load(textures_[DIAMOND][STRIPED],
             "assets/diamond_striped.png");

        load(textures_[DIAMOND][SOLID],
             "assets/diamond_solid.png");

        load(textures_[OVAL][OPEN],
             "assets/oval_open.png");

        load(textures_[OVAL][STRIPED],
             "assets/oval_striped.png");

        load(textures_[OVAL][SOLID],
             "assets/oval_solid.png");

        load(textures_[SQUIGGLE][OPEN],
             "assets/squiggle_open.png");

        load(textures_[SQUIGGLE][STRIPED],
             "assets/squiggle_striped.png");

        load(textures_[SQUIGGLE][SOLID],
             "assets/squiggle_solid.png");
    }

    void SDLRenderer::run(core::Game &game) {
        bool running = true;

        while (running) {
            processEvents(running, game);
            update(game);
            render(game);
            SDL_Delay(16);
        }
    }

    void SDLRenderer::processEvents(bool &running, core::Game &game) {
        SDL_Event event;

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = false;

            if (event.type ==
                SDL_MOUSEBUTTONDOWN &&
                !animatingRemoval_) {
                handleClick(
                    event.button.x,
                    event.button.y,
                    game
                );
            }
        }
    }

    void SDLRenderer::handleClick(
        int mouseX,
        int mouseY,
        core::Game &game) {
        const auto &cards =
                game.board().cards();

        for (size_t i = 0; i < cards.size(); i++) {
            SDL_Rect rect = getCardRect(i);

            bool inside = mouseX >= rect.x &&
                          mouseX <= rect.x + rect.w &&
                          mouseY >= rect.y &&
                          mouseY <= rect.y + rect.h;

            if (!inside) continue;

            auto it =
                    std::find(
                        selected_.begin(),
                        selected_.end(),
                        i
                    );

            if (it != selected_.end()) {
                selected_.erase(it);
                return;
            }

            if (selected_.size() < 3) {
                selected_.push_back(i);
            }

            if (selected_.size() == 3) {
                const auto &c0 = cards[selected_[0]];
                const auto &c1 = cards[selected_[1]];
                const auto &c2 = cards[selected_[2]];

                bool valid = core::isValidSet(c0, c1, c2);

                if (valid) {
                    beginSetRemoval();
                } else {
                    selected_.clear();
                }
            }

            return;
        }
    }

    void SDLRenderer::beginSetRemoval() {
        animatingRemoval_ = true;
        animationStart_ = SDL_GetTicks();
    }

    void SDLRenderer::update(core::Game &game) {
        if (!animatingRemoval_) return;

        Uint32 elapsed = SDL_GetTicks() - animationStart_;

        if (elapsed >= ANIMATION_DURATION_MS) {
            finishSetRemoval(game);
            animatingRemoval_ = false;
        }
    }

    void SDLRenderer::finishSetRemoval(core::Game &game) {
        game.replaceSelectedCards(selected_);
        game.refillBoard();
        selected_.clear();
    }

    bool SDLRenderer::isSelected(int index) const {
        return std::find(
                   selected_.begin(),
                   selected_.end(),
                   index)
               != selected_.end();
    }

    SDL_Rect SDLRenderer::getCardRect(int index) const {
        int row = index / GRID_COLUMNS;
        int col = index % GRID_COLUMNS;

        constexpr int START_X = 80;
        constexpr int START_Y = 60;

        int x = START_X + col * (CARD_WIDTH + CARD_SPACING_X);
        int y = START_Y + row * (CARD_HEIGHT + CARD_SPACING_Y);

        return {x, y, CARD_WIDTH, CARD_HEIGHT};
    }

    void SDLRenderer::render(const core::Game &game) {
        SDL_SetRenderDrawColor(
            renderer_,
            30, 30, 30, 255
        );

        SDL_RenderClear(renderer_);

        const auto &cards = game.board().cards();

        for (size_t i = 0; i < cards.size(); i++) {
            renderCard(cards[i], i);
        }

        SDL_RenderPresent(renderer_);
    }

    void SDLRenderer::renderCard(const core::Card &card, int index) {
        SDL_Rect rect = getCardRect(index);

        bool selected = isSelected(index);

        if (selected) {
            SDL_SetRenderDrawColor(
                renderer_,
                100, 180, 255, 255
            );
        } else {
            SDL_SetRenderDrawColor(
                renderer_,
                255, 255, 255, 255
            );
        }

        SDL_RenderFillRect(renderer_, &rect);

        SDL_SetRenderDrawColor(
            renderer_,
            0, 0, 0, 255
        );

        SDL_RenderDrawRect(renderer_, &rect);

        int number = card[0];
        int color = card[1];
        int shading = card[2];
        int shape = card[3];

        SDL_Texture *texture = textures_[shape][shading];

        if (!texture) return;

        SDL_Color tint = getColor(color);
        int count = number + 1;
        int texW, texH;

        SDL_QueryTexture(
            texture,
            nullptr,
            nullptr,
            &texW,
            &texH
        );

        float targetH = rect.h * 0.5f;
        float scale = targetH / texH;
        int w = int(texW * scale);
        int h = int(texH * scale);
        int spacing = 12;
        int totalWidth = count * w + (count - 1) * spacing;
        int startX = rect.x + rect.w / 2 - totalWidth / 2;

        for (int i = 0; i < count; i++) {
            int symbolX = startX + i * (w + spacing);

            SDL_Rect dst{
                symbolX,
                rect.y + rect.h / 2 - h / 2,
                w,
                h
            };

            if (animatingRemoval_ && isSelected(index)) {
                Uint32 elapsed = SDL_GetTicks() - animationStart_;

                float t = 1.0f - (float(elapsed) / ANIMATION_DURATION_MS);

                dst.w *= t;
                dst.h *= t;

                dst.x += (w - dst.w) / 2;
                dst.y += (h - dst.h) / 2;
            }

            SDL_SetTextureColorMod(
                texture,
                tint.r,
                tint.g,
                tint.b
            );

            SDL_RenderCopy(
                renderer_,
                texture,
                nullptr,
                &dst
            );
        }
    }
} // namespace setgame::ui
