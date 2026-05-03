#pragma once
#include "Monster.h"

// Boss is derived from Monster; holds the Secret Stone
// Defeating the boss and collecting the Secret Stone ends the game
class Boss : public Monster {
public:
    Boss() {}
    Boss(const std::string& n, int hp, int sk, int ar, int pw, int b,
         const std::string& desc, const std::string& song)
        : Monster(n, hp, sk, ar, pw, b, desc, song) {}
    virtual ~Boss() {}
};
