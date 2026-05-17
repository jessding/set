#ifndef RULES_H
#define RULES_H

#include <array>

namespace setgame {

using Card = std::array<uint8_t, 4>;

bool isValidSet(const Card& x, const Card& y, const Card& z);

Card getThirdCard(const Card& x, const Card& y);

} // namespace setgame

#endif