#pragma once
#include "Item.h"

// Potion restores hitpoints when consumed
class Potion : public Item {
private:
    int strength;
public:
    Potion() : strength(0) {}
    Potion(const std::string& n, int s) : Item(n), strength(s) {}

    void setStrength(int hp) { strength = hp; }
    int getStrength() const { return strength; }
};
