#include "Character.h"
#include "Item.h"
#include "Potion.h"
#include "Weapon.h"
#include "Treasure.h"
#include "Armour.h"
#include "Location.h"
#include <algorithm>
#include <sstream>

Character::Character() : hitpoints(0), skill(0), armour(0), location(nullptr) {}

Character::Character(const std::string& n, int hp, int sk, int ar)
    : name(n), hitpoints(hp), skill(sk), armour(ar), location(nullptr) {}

// Compute total armour protection from all Armour items in inventory
int Character::getArmourProtection() const {
    int total = 0;
    for (Item* item : inventory) {
        Armour* a = dynamic_cast<Armour*>(item);
        if (a) total += a->getProtection();
    }
    return total;
}

void Character::addItem(Item* item) {
    inventory.push_back(item);
}

void Character::removeItem(Item* item) {
    inventory.erase(std::remove(inventory.begin(), inventory.end(), item), inventory.end());
}

// Drop item from inventory into the character's current location
void Character::dropItem(Item* item) {
    if (location) {
        location->addItem(item);
    }
    removeItem(item);
}

bool Character::moveTo(Location* newLocation) {
    if (newLocation) {
        location = newLocation;
        return true;
    }
    return false;
}

// Reduce hitpoints by the hit value; clamp to 0
void Character::takeHit(int hit) {
    hitpoints -= hit;
    if (hitpoints < 0) hitpoints = 0;
}

// Returns a categorised, alphabetically sorted inventory string
std::string Character::getInventory() const {
    std::vector<std::string> potions, treasures, weapons, armours;

    for (Item* item : inventory) {
        if (dynamic_cast<Potion*>(item))       potions.push_back(item->getName());
        else if (dynamic_cast<Armour*>(item))  armours.push_back(item->getName());
        else if (dynamic_cast<Weapon*>(item))  weapons.push_back(item->getName());
        else if (dynamic_cast<Treasure*>(item)) treasures.push_back(item->getName());
    }

    std::sort(potions.begin(),   potions.end());
    std::sort(treasures.begin(), treasures.end());
    std::sort(weapons.begin(),   weapons.end());
    std::sort(armours.begin(),   armours.end());

    std::ostringstream oss;
    oss << "=== Inventory ===\n";

    oss << "[Potions]\n";
    if (potions.empty()) oss << "  (none)\n";
    else for (const auto& s : potions) oss << "  " << s << "\n";

    oss << "[Treasures]\n";
    if (treasures.empty()) oss << "  (none)\n";
    else for (const auto& s : treasures) oss << "  " << s << "\n";

    oss << "[Weapons]\n";
    if (weapons.empty()) oss << "  (none)\n";
    else for (const auto& s : weapons) oss << "  " << s << "\n";

    oss << "[Armour]\n";
    if (armours.empty()) oss << "  (none)\n";
    else for (const auto& s : armours) oss << "  " << s << "\n";

    return oss.str();
}
