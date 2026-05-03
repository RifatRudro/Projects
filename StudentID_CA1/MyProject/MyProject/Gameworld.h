#pragma once
#include <vector>
#include <map>

class Location;
class Item;
class Monster;
class Player;

// Gameworld holds all locations and manages game state
class Gameworld {
private:
    bool gameOver;
    std::map<int, Location*> locations; // ID -> Location*

public:
    Gameworld();
    ~Gameworld();

    bool isGameOver() const { return gameOver; }
    void setGameOver(bool go) { gameOver = go; }

    // Creates all locations, links exits, places items and monsters
    void loadWorld();

    // Returns starting location for the player
    Location* getStartLocation();
};
