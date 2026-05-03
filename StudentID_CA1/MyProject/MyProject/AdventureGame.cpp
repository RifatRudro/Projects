#include <iostream>
#include <string>
#include <algorithm>
#include <cstdlib>
#include <ctime>

#include "Gameworld.h"
#include "Player.h"
#include "Location.h"
#include "Item.h"
#include "Treasure.h"
#include "Monster.h"

// Convert a string to lowercase for case-insensitive command matching
static std::string toLower(const std::string& s) {
    std::string result = s;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

// Check if the player has collected the Secret Stone (game win condition)
static bool hasSecretStone(Player& player) {
    for (Item* item : player.getInventoryList()) {
        if (item->getName() == "Secret Stone") return true;
    }
    return false;
}

// Print current location info: name, description, exits, items, and monsters
static void describeLocation(const Player& player) {
    Location* loc = player.getLocation();
    if (!loc) return;

    std::cout << "\n============================================\n";
    std::cout << "[ " << loc->getName() << " ]\n";
    std::cout << loc->getDescription() << "\n";
    std::cout << loc->showExits() << "\n";

    // Show monsters present
    const std::vector<Monster*>& monsters = const_cast<Location*>(loc)->getMonsters();
    if (!monsters.empty()) {
        std::cout << "Enemies: ";
        for (size_t i = 0; i < monsters.size(); ++i) {
            if (i > 0) std::cout << ", ";
            std::cout << monsters[i]->getName()
                      << " (HP: " << monsters[i]->getHitpoints() << ")";
        }
        std::cout << "\n";
    }

    // Show items on the ground
    std::string items = loc->showItems();
    if (!items.empty()) std::cout << items << "\n";
    std::cout << "============================================\n";
}

int main() {
    srand(static_cast<unsigned int>(time(nullptr)));

    // -------------------------------------------------------
    // Introductory scene-setting text
    // -------------------------------------------------------
    std::cout << "============================================\n";
    std::cout << "   THE LEGEND OF ZELDA: ECHOES OF GLOOM\n";
    std::cout << "============================================\n";
    std::cout << "\nHyrule has been shaken by the Upheaval.\n";
    std::cout << "Strange ruins have risen from the earth, and a creeping Gloom\n";
    std::cout << "seeps across the land. Monsters roam freely where peace once held.\n";
    std::cout << "\nYou are Link, a skilled warrior tasked with restoring order.\n";
    std::cout << "Travel the fractured kingdom, gather weapons and potions,\n";
    std::cout << "defeat the monsters that terrorise Hyrule, and ultimately\n";
    std::cout << "confront the source of the darkness.\n";
    std::cout << "\nThe Demon King is said to dwell within the Gloom-Fortified Spire.\n";
    std::cout << "Defeat him and seize the Secret Stone to save Hyrule!\n";
    std::cout << "\nCommands: north/n, south/s, east/e, west/w, inventory/inv,\n";
    std::cout << "          fight/f, collect/c, drink/d, quit/q\n";
    std::cout << "============================================\n\n";

    // -------------------------------------------------------
    // Initialise game world and player
    // -------------------------------------------------------
    Gameworld world;
    world.loadWorld();

    Player player("Link", 50, 6, 0);
    player.setLocation(world.getStartLocation());

    // -------------------------------------------------------
    // Main game loop (Tasks 3-7)
    // -------------------------------------------------------
    describeLocation(player);

    while (!world.isGameOver()) {
        std::cout << "\n[HP: " << player.getHitpoints()
                  << " | Score: " << player.getScore()
                  << "] > ";

        std::string input;
        if (!std::getline(std::cin, input)) {
            world.setGameOver(true);
            break;
        }

        std::string cmd = toLower(input);
        // Trim leading/trailing whitespace
        cmd.erase(0, cmd.find_first_not_of(" \t"));
        if (!cmd.empty()) cmd.erase(cmd.find_last_not_of(" \t") + 1);

        if (cmd.empty()) continue;

        // --- Navigation commands ---
        if (cmd == "n" || cmd == "north" ||
            cmd == "s" || cmd == "south" ||
            cmd == "e" || cmd == "east"  ||
            cmd == "w" || cmd == "west") {

            bool moved = player.moveTo(cmd);
            if (moved) {
                describeLocation(player);
            } else {
                std::cout << "There is no exit in that direction.\n";
            }

        // --- Inventory ---
        } else if (cmd == "inventory" || cmd == "inv") {
            std::cout << player.getInventory();

        // --- Collect items ---
        } else if (cmd == "collect" || cmd == "c") {
            Location* loc = player.getLocation();
            if (loc && loc->getItems().empty()) {
                std::cout << "There is nothing here to collect.\n";
            } else {
                std::vector<Item*> before = loc->getItems(); // copy to list names
                player.collectItems(loc);
                if (!before.empty()) {
                    std::cout << "You collected: ";
                    for (size_t i = 0; i < before.size(); ++i) {
                        if (i > 0) std::cout << ", ";
                        std::cout << before[i]->getName();
                    }
                    std::cout << "\n";

                    // Task 7: Check if Secret Stone was just collected
                    if (hasSecretStone(player)) {
                        std::cout << "\n*** You hold the Secret Stone! ***\n";
                        std::cout << "A warm golden light radiates from your hands.\n";
                        std::cout << "The Gloom retreats. Hyrule is saved!\n";
                        std::cout << "Final Score: " << player.getScore() << "\n";
                        world.setGameOver(true);
                    }
                }
            }

        // --- Drink potions ---
        } else if (cmd == "drink" || cmd == "d") {
            std::cout << player.drinkPotions();

        // --- Fight ---
        } else if (cmd == "fight" || cmd == "f") {
            std::string result = player.combat();
            std::cout << result;

            // Check if player was defeated
            if (player.getHitpoints() <= 0) {
                world.setGameOver(true);
            }

        // --- Quit ---
        } else if (cmd == "quit" || cmd == "q") {
            std::cout << "Farewell, brave hero. Perhaps another day...\n";
            std::cout << "Final Score: " << player.getScore() << "\n";
            world.setGameOver(true);

        } else {
            std::cout << "Unknown command: '" << cmd << "'. Try: n/s/e/w, inventory, collect, drink, fight, quit\n";
        }
    }

    std::cout << "\nGame Over. Thanks for playing!\n";
    return 0;
}
