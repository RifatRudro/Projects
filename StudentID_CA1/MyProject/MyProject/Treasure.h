#pragma once
#include "Item.h"

// Treasure has a monetary value
class Treasure : public Item {
private:
    int value;
public:
    Treasure() : value(0) {}
    Treasure(const std::string& n, int v) : Item(n), value(v) {}

    void setValue(int val) { value = val; }
    int getValue() const { return value; }
};
