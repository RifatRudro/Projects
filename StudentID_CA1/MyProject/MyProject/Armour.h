#pragma once
#include "Item.h"

// Armour reduces incoming damage
class Armour : public Item {
private:
    int protection;
public:
    Armour() : protection(0) {}
    Armour(const std::string& n, int p) : Item(n), protection(p) {}

    void setProtection(int val) { protection = val; }
    int getProtection() const { return protection; }
};
