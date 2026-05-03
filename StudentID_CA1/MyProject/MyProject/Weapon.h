#pragma once
#include "Item.h"

// Weapon is used in combat; higher power = more damage
class Weapon : public Item {
private:
    int power;
public:
    Weapon() : power(0) {}
    Weapon(const std::string& n, int p) : Item(n), power(p) {}

    void setPower(int ap) { power = ap; }
    int getPower() const { return power; }
};
