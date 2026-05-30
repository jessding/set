#include "core/rules.h"

namespace setgame::core {
    bool isValidSet(const Card &x, const Card &y, const Card &z) {
        for (int i = 0; i < 4; i++) {
            if ((x[i] + y[i] + z[i]) % 3 != 0) return false;
        }
        return true;
    }

    Card getThirdCard(const Card &x, const Card &y) {
        Card z{};

        for (int i = 0; i < 4; i++) {
            z[i] = (6 - x[i] - y[i]) % 3;
        }
        return z;
    }
} // namespace setgame::core
