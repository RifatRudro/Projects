#include "Player.h"
#include "Location.h"
#include "Item.h"
#include "Potion.h"
#include "Weapon.h"
#include "Armour.h"
#include "Monster.h"
#include <algorithm>
#include <sstream>
#include <cstdlib>
#include <ctime>

// Rolls a single six-sided die
static int rollDice() {
    return (rand() % 6) + 1;
}

Player::Player() : score(0) {}

Player::Player(const std::string& n, int hp, int sk, int ar)
    : Character(n, hp, sk, ar), score(0) {}

void Player::setLocation(Location* loc) {
    location = loc;
}

Location* Player::getLocation() const {
    return location;
}

// Collect all items in the given location into the player's inventory
void Player::collectItems(Location* loc) {
    if (!loc) return;
    std::vector<Item*> toCollect = loc->getItems();
    for (Item* item : toCollect) {
        inventory.push_back(item);
        loc->removeItem(item);
    }
}

// Attempt to move the player in the given direction
// Accepts full words or single letters, case-insensitive
bool Player::moveTo(const std::string& direction) {
    if (!location) return false;

    // Normalise to uppercase full direction name
    std::string dir = direction;
    for (char& c : dir) c = toupper(c);

    std::string fullDir;
    if      (dir == "N" || dir == "NORTH") fullDir = "NORTH";
    else if (dir == "S" || dir == "SOUTH") fullDir = "SOUTH";
    else if (dir == "E" || dir == "EAST")  fullDir = "EAST";
    else if (dir == "W" || dir == "WEST")  fullDir = "WEST";
    else return false;

    Location* next = location->getExit(fullDir);
    if (next) {
        location = next;
        return true;
    }
    return false;
}

// Drink all potions in inventory; returns a description of what happened
std::string Player::drinkPotions() {
    std::ostringstream oss;
    std::vector<Item*> toRemove;

    for (Item* item : inventory) {
        Potion* p = dynamic_cast<Potion*>(item);
        if (p) {
            hitpoints += p->getStrength();
            oss << "You drink the " << p->getName()
                << " and recover " << p->getStrength() << " HP. "
                << "(HP now: " << hitpoints << ")\n";
            toRemove.push_back(item);
        }
    }

    for (Item* item : toRemove) {
        removeItem(item);
        delete item;
    }

    if (toRemove.empty()) {
        oss << "You have no potions to drink.";
    }
    return oss.str();
}

// Conduct a full combat sequence against the strongest monster in the current location
// Returns a string log of the entire fight
std::string Player::combat() {
    if (!location) return "You are nowhere. How is that possible?";

    std::vector<Monster*>& monsters = location->getMonsters();
    if (monsters.empty()) return "There are no enemies here to fight.";

    // Fight the monster with the highest current HP (strongest first)
    Monster* enemy = nullptr;
    for (Monster* m : monsters) {
        if (!enemy || m->getHitpoints() > enemy->getHitpoints()) {
            enemy = m;
        }
    }

    std::ostringstream oss;
    oss << "\n*** COMBAT: " << getName() << " vs " << enemy->getName() << " ***\n";
    oss << enemy->getDescription() << "\n\n";

    // Find player's best weapon
    Weapon* bestWeapon = nullptr;
    for (Item* item : inventory) {
        Weapon* w = dynamic_cast<Weapon*>(item);
        if (w) {
            if (!bestWeapon || w->getPower() > bestWeapon->getPower()) {
                bestWeapon = w;
            }
        }
    }

    int playerWeaponPower = bestWeapon ? bestWeapon->getPower() : 0;
    if (bestWeapon)
        oss << "You are wielding: " << bestWeapon->getName()
            << " (Power: " << playerWeaponPower << ")\n\n";
    else
        oss << "You are unarmed!\n\n";

    int round = 1;
    while (hitpoints > 0 && enemy->getHitpoints() > 0) {
        oss << "-- Round " << round++ << " --\n";

        // Resolve attack rolls; re-roll on draws
        int playerAttack, enemyAttack;
        do {
            int playerRoll = rollDice() + rollDice();
            playerAttack   = skill + playerWeaponPower + playerRoll;

            int enemyRoll  = rollDice() + rollDice();
            enemyAttack    = enemy->getSkill() + enemyRoll;

            oss << "  You roll " << playerRoll
                << " -> attack total: " << playerAttack << "\n";
            oss << "  " << enemy->getName() << " rolls " << enemyRoll
                << " -> attack total: " << enemyAttack << "\n";

            if (playerAttack == enemyAttack)
                oss << "  Draw! Rolling again...\n";
        } while (playerAttack == enemyAttack);

        if (playerAttack > enemyAttack) {
            // Player wins the round
            int rawDamage = (bestWeapon ? playerWeaponPower + 6 : 1);
            int armourReduction = enemy->getArmourProtection() + enemy->getArmour();
            int damage = std::max(1, rawDamage - armourReduction);
            enemy->takeHit(damage);
            oss << "  You hit the " << enemy->getName()
                << " for " << damage << " damage! "
                << "(Enemy HP: " << enemy->getHitpoints() << ")\n";
        } else {
            // Monster wins the round
            int rawDamage = enemy->getPower();
            int armourReduction = getArmourProtection() + armour;
            int damage = std::max(1, rawDamage - armourReduction);
            takeHit(damage);
            oss << "  " << enemy->getName()
                << " hits you for " << damage << " damage! "
                << "(Your HP: " << hitpoints << ")\n";
        }
        oss << "\n";
    }

    if (enemy->getHitpoints() <= 0) {
        // Monster defeated
        oss << enemy->getSwansong() << "\n";
        oss << "You defeated " << enemy->getName() << "!\n";
        score += enemy->getBounty();
        oss << "Score +" << enemy->getBounty()
            << " (Total: " << score << ")\n";

        // Drop all items into the location
        std::vector<Item*> drops = enemy->getInventoryList();
        for (Item* item : drops) {
            location->addItem(item);
            oss << "  " << enemy->getName()
                << " dropped: " << item->getName() << "\n";
        }
        enemy->getInventoryList().clear();
        location->delMonster(enemy);
        delete enemy;
    } else {
        // Player defeated
        oss << "\nYou have been defeated...\n";
        oss << "Final score: " << score << "\n";
    }

    return oss.str();
}
