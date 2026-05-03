#include "Location.h"
#include "Item.h"
#include "Monster.h"
#include <algorithm>
#include <sstream>


void Location::addExit(const std::string& direction, Location* loc) {
    exits[direction] = loc;
}

std::string Location::showExits() const {
    std::string result = "Exits: ";
    bool first = true;
    std::vector<std::string> order = {"NORTH", "SOUTH", "EAST", "WEST"};
    for (const auto& dir : order) {
        if (exits.count(dir)) {
            if (!first) result += ", ";
            result += dir;
            first = false;
        }
    }
    if (first) result += "None";
    return result;
}

Location* Location::getExit(const std::string& direction) const {
    auto it = exits.find(direction);
    if (it != exits.end()) return it->second;
    return nullptr;
}

void Location::addMonster(Monster* monster) {
    monsters.push_back(monster);
}

void Location::delMonster(Monster* monster) {
    monsters.erase(std::remove(monsters.begin(), monsters.end(), monster), monsters.end());
}

void Location::addItem(Item* item) {
    items.push_back(item);
}

void Location::removeItem(Item* item) {
    items.erase(std::remove(items.begin(), items.end(), item), items.end());
}

std::string Location::showItems() const {
    if (items.empty()) return "";
    std::string result = "Items here: ";
    for (size_t i = 0; i < items.size(); ++i) {
        if (i > 0) result += ", ";
        result += items[i]->getName();
    }
    return result;
}
