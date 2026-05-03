#pragma once
#include <string>
#include <map>
#include <vector>

class Item;
class Monster;

// Location represents a place in the game world
class Location {
private:
    std::string name;
    std::string description;
    std::map<std::string, Location*> exits; // direction -> location
    std::vector<Item*> items;
    std::vector<Monster*> monsters;

public:
    Location() {}
    Location(const std::string& n, const std::string& desc) : name(n), description(desc) {}
    ~Location() {}

    void setName(const std::string& newName) { name = newName; }
    std::string getName() const { return name; }

    void setDescription(const std::string& newDesc) { description = newDesc; }
    std::string getDescription() const { return description; }

    // Add an exit in the given direction leading to another location
    void addExit(const std::string& direction, Location* loc);

    // Returns a formatted string of available exits
    std::string showExits() const;

    // Get the location in the given direction (nullptr if none)
    Location* getExit(const std::string& direction) const;

    void addMonster(Monster* monster);
    void delMonster(Monster* monster);
    std::vector<Monster*>& getMonsters() { return monsters; }

    void addItem(Item* item);
    void removeItem(Item* item);
    std::vector<Item*>& getItems() { return items; }

    // Returns string listing items in this location, or empty string
    std::string showItems() const;
};
