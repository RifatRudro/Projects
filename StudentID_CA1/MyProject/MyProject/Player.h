#pragma once
#include "Character.h"
#include <string>

// Player is the human-controlled character
class Player : public Character {
private:
    int score;

public:
    Player();
    Player(const std::string& n, int hp, int sk, int ar);
    virtual ~Player() {}

    void setScore(int s) { score = s; }
    int getScore() const { return score; }

    // Override setLocation / getLocation so Player tracks its own location
    void setLocation(Location* loc);
    Location* getLocation() const;

    // Collect all items from the given location into inventory
    void collectItems(Location* loc);

    // Move in the given direction; returns true if successful
    bool moveTo(const std::string& direction);

    // Drink all potions in inventory; returns result description
    std::string drinkPotions();

    // Engage combat with a monster in the current location
    std::string combat();
};
