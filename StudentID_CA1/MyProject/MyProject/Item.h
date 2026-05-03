#pragma once
#include <string>

// Base class for all in-game items
class Item {
protected:
    std::string name;
public:
    Item() {}
    Item(const std::string& n) : name(n) {}
    virtual ~Item() {}

    void setName(const std::string& newName) { name = newName; }
    std::string getName() const { return name; }
};
