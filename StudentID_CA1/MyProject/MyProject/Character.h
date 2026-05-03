#pragma once
#include <string>
#include <vector>

class Item;
class Location;

// Base class for all characters (players and monsters)
class Character {
protected:
    std::string name;
    std::string description;
    int hitpoints;
    int skill;
    int armour;       // base armour value (not item-based)
    Location* location;
    std::vector<Item*> inventory;

public:
    Character();
    Character(const std::string& n, int hp, int sk, int ar);
    virtual ~Character() {}

    // Name accessors
    void setName(const std::string& newName) { name = newName; }
    std::string getName() const { return name; }

    // Description accessors
    void setDescription(const std::string& desc) { description = desc; }
    std::string getDescription() const { return description; }

    // Hitpoints
    void setHitpoints(int hp) { hitpoints = hp; }
    int getHitpoints() const { return hitpoints; }

    // Skill
    void setSkill(int sk) { skill = sk; }
    int getSkill() const { return skill; }

    // Armour (base value, separate from item armour)
    void setArmour(int ar) { armour = ar; }
    int getArmour() const { return armour; }

    // Calculates total armour protection from all Armour items in inventory
    int getArmourProtection() const;

    // Inventory management
    void addItem(Item* item);
    void removeItem(Item* item);
    void dropItem(Item* item); // drops item into current location

    // Location
    Location* getLocation() const { return location; }
    bool moveTo(Location* newLocation);

    // Reduce hitpoints by the hit amount
    void takeHit(int hit);

    // Returns formatted inventory string grouped by type
    std::string getInventory() const;

    std::vector<Item*>& getInventoryList() { return inventory; }
};
