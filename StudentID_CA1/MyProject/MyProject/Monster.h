#pragma once
#include "Character.h"

// Monster is a non-player character that can fight the player
class Monster : public Character {
protected:
    int bounty;
    int power;
    std::string swansong;

public:
    Monster() : bounty(0), power(0) {}
    Monster(const std::string& n, int hp, int sk, int ar, int pw, int b,
            const std::string& desc, const std::string& song)
        : Character(n, hp, sk, ar), bounty(b), power(pw), swansong(song)
    {
        description = desc;
    }
    virtual ~Monster() {}

    std::string getSwansong() const { return swansong; }
    void setSwansong(const std::string& newSwansong) { swansong = newSwansong; }

    int getBounty() const { return bounty; }
    void setBounty(int b) { bounty = b; }

    void setPower(int pw) { power = pw; }
    int getPower() const { return power; }
};
