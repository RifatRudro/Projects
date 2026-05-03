#include "Gameworld.h"
#include "Location.h"
#include "Item.h"
#include "Potion.h"
#include "Weapon.h"
#include "Treasure.h"
#include "Armour.h"
#include "Monster.h"
#include "Boss.h"
#include <iostream>

Gameworld::Gameworld() : gameOver(false) {}

Gameworld::~Gameworld() {
    // Free all allocated locations
    for (auto& pair : locations) {
        delete pair.second;
    }
    locations.clear();
}

void Gameworld::loadWorld() {
    // -------------------------------------------------------
    // Task 2: Create all 17 locations
    // -------------------------------------------------------
    locations[19] = new Location("Lookout Landing",
        "A busy outpost where travellers, soldiers, and researchers regroup after the Upheaval.");
    locations[1]  = new Location("Hyrule Field Gate",
        "A battered stone gate opening onto Hyrule Field, marked with old royal crests.");
    locations[2]  = new Location("Purahs Research Camp",
        "A Sheikah-led camp of notes, lanterns, and humming devices, tracking strange activity across Hyrule.");
    locations[5]  = new Location("Skyview Tower",
        "A towering launch point with creaking scaffolds and a view that stretches across the horizon.");
    locations[30] = new Location("Ruins of the Great Plateau",
        "Wind-scoured stone and ancient shrines, still watching over the land.");
    locations[29] = new Location("The Depths Chasm",
        "A yawning chasm seeping gloom, descending into the darkness below.");
    locations[32] = new Location("Forgotten Zonai Sanctum",
        "A sealed chamber of ancient mechanisms, pulsing with a faint green light.");
    locations[31] = new Location("Zonai Sky Ruins",
        "Floating fragments of stone and circuitry, humming softly as if still alive.");
    locations[20] = new Location("Gloom-Fortified Spire",
        "A corrupted stronghold wrapped in gloom, where a powerful presence waits.");
    locations[13] = new Location("Hyrule Castle Library",
        "Collapsed shelves and scattered tomes, half-buried under ash and ivy.");
    locations[15] = new Location("Hateno Ancient Tech Lab",
        "A quiet workshop of tools, diagrams, and half-finished prototypes.");
    locations[3]  = new Location("The Lost Temple",
        "An ancient temple swallowed by time, its halls lit by cold, blue light.");
    locations[6]  = new Location("Korok Forest",
        "A sheltered woodland clearing, where rustling leaves feel like whispered laughter.");
    locations[14] = new Location("Akkala Citadel",
        "A red-stone fortress overlooking the land, still scarred by past battles.");
    locations[9]  = new Location("Gerudo Training Grounds",
        "Sun-baked stone and worn targets, where the air still carries the rhythm of combat drills.");
    locations[7]  = new Location("Coliseum Ruins",
        "A circular arena of cracked pillars and sand, built for champions and monsters alike.");
    locations[26] = new Location("Goron Forgeworks",
        "A blazing workshop of anvils and ore, where weapons are reforged for the road ahead.");

    // -------------------------------------------------------
    // Task 2: Link exits according to the spec table
    // -------------------------------------------------------
    locations[19]->addExit("WEST",  locations[1]);
    locations[19]->addExit("NORTH", locations[2]);
    locations[19]->addExit("EAST",  locations[3]);

    locations[1]->addExit("EAST",   locations[19]);

    locations[2]->addExit("SOUTH",  locations[19]);
    locations[2]->addExit("NORTH",  locations[5]);
    locations[2]->addExit("EAST",   locations[13]);
    locations[2]->addExit("WEST",   locations[15]);

    locations[5]->addExit("SOUTH",  locations[2]);
    locations[5]->addExit("NORTH",  locations[30]);
    locations[5]->addExit("EAST",   locations[31]);

    locations[30]->addExit("SOUTH", locations[5]);
    locations[30]->addExit("WEST",  locations[29]);

    locations[29]->addExit("EAST",  locations[30]);
    locations[29]->addExit("SOUTH", locations[32]);

    locations[32]->addExit("NORTH", locations[29]);

    locations[31]->addExit("NORTH", locations[20]);
    locations[31]->addExit("WEST",  locations[5]);

    locations[20]->addExit("SOUTH", locations[31]);

    locations[13]->addExit("WEST",  locations[2]);

    locations[15]->addExit("EAST",  locations[2]);

    locations[3]->addExit("WEST",   locations[19]);
    locations[3]->addExit("NORTH",  locations[6]);
    locations[3]->addExit("EAST",   locations[14]);
    locations[3]->addExit("SOUTH",  locations[9]);

    locations[6]->addExit("SOUTH",  locations[3]);
    locations[6]->addExit("EAST",   locations[26]);

    locations[14]->addExit("SOUTH", locations[7]);
    locations[14]->addExit("NORTH", locations[26]);
    locations[14]->addExit("WEST",  locations[3]);

    locations[9]->addExit("NORTH",  locations[3]);
    locations[9]->addExit("WEST",   locations[7]);

    locations[7]->addExit("NORTH",  locations[14]);
    locations[7]->addExit("EAST",   locations[9]);

    locations[26]->addExit("SOUTH", locations[14]);
    locations[26]->addExit("WEST",  locations[6]);

    // -------------------------------------------------------
    // Task 4: Create monsters and place them in locations
    // -------------------------------------------------------

    // Yiga Footsoldier (Location 6)
    Monster* yiga = new Monster("Yiga Footsoldier", 30, 3, 5, 10, 25,
        "A masked ambusher that strikes quickly and disappears if the fight turns.",
        "A puff of smoke, then silence.");
    yiga->addItem(new Weapon("Duplex Bow", 15));
    yiga->addItem(new Armour("Yiga Tights", 5));
    locations[6]->addMonster(yiga);

    // Moblin (Location 5)
    Monster* moblin = new Monster("Moblin", 40, 4, 8, 12, 30,
        "A towering brute that fights with heavy swings and stubborn momentum.",
        "It staggers, groans, and crashes down.");
    moblin->addItem(new Weapon("Royal Broadsword", 12));
    moblin->addItem(new Treasure("Ancient Spring", 20));
    locations[5]->addMonster(moblin);

    // Hinox (Location 29)
    Monster* hinox = new Monster("Hinox", 50, 2, 10, 15, 50,
        "A one-eyed giant that relies on raw strength and trampling blows.",
        "It bellows once, then collapses.");
    hinox->addItem(new Treasure("Amber", 100));
    hinox->addItem(new Potion("Hearty Elixir", 40));
    locations[29]->addMonster(hinox);

    // Stone Talus (Location 29)
    Monster* talus = new Monster("Stone Talus", 60, 2, 12, 18, 60,
        "An ancient creature of living rock that hits like a falling boulder.",
        "The glow fades, and the stones fall apart.");
    talus->addItem(new Treasure("Ruby", 150));
    locations[29]->addMonster(talus);

    // Bokoblin (Location 2)
    Monster* bokoblin = new Monster("Bokoblin", 45, 4, 7, 20, 35,
        "A sneaky scavenger that lunges in when you're off-balance.",
        "It yelps and slumps to the ground.");
    bokoblin->addItem(new Weapon("Travelers Sword", 15));
    bokoblin->addItem(new Treasure("Red Rupee", 50));
    bokoblin->addItem(new Armour("Bokoblin Mask", 20));
    locations[2]->addMonster(bokoblin);

    // Gibdo (Location 30)
    Monster* gibdo = new Monster("Gibdo", 35, 2, 4, 8, 20,
        "A cursed husk that shambles forward, unnervingly stubborn.",
        "It crumbles into dust and is carried off by the wind.");
    gibdo->addItem(new Treasure("Dark Clump", 75));
    gibdo->addItem(new Potion("Fairy Tonic", 50));
    locations[30]->addMonster(gibdo);

    // Construct (Location 31)
    Monster* construct = new Monster("Construct", 50, 5, 12, 18, 40,
        "An ancient guardian construct that fights with precise, mechanical strikes.",
        "Its lights flicker... then go out.");
    construct->addItem(new Treasure("Zonaite", 20));
    construct->addItem(new Weapon("Soldiers Bow", 5));
    locations[31]->addMonster(construct);

    // Keese (Location 9)
    Monster* keese = new Monster("Keese", 25, 1, 3, 10, 15,
        "A fluttering nuisance that dives from the dark in sudden swarms.",
        "Its wings go still as it drops.");
    keese->addItem(new Treasure("Arrow Bundle", 20));
    keese->addItem(new Treasure("Blue Rupee", 30));
    keese->addItem(new Treasure("Opal", 40));
    locations[9]->addMonster(keese);

    // -------------------------------------------------------
    // Task 7: Boss - Ganondorf at Gloom-Fortified Spire (20)
    // -------------------------------------------------------
    Boss* ganondorf = new Boss("Ganondorf", 120, 8, 15, 30, 200,
        "The Demon King himself, wreathed in gloom and malice. His eyes burn with ancient fury.",
        "The Demon King lets out a final, earth-shaking roar... then the darkness fades.");
    ganondorf->addItem(new Treasure("Secret Stone", 200));
    locations[20]->addMonster(ganondorf);

    // -------------------------------------------------------
    // Task 5: Place items in locations
    // -------------------------------------------------------

    // Lookout Landing (19)
    locations[19]->addItem(new Potion("Green Potion", 30));

    // Purah's Research Camp (2)
    locations[2]->addItem(new Treasure("Sheikah Heirloom", 25));

    // Hateno Ancient Tech Lab (15)
    locations[15]->addItem(new Treasure("Topaz", 50));

    // Forgotten Zonai Sanctum (32)
    locations[32]->addItem(new Potion("Chateau Romani", 200));

    // Akkala Citadel (14)
    locations[14]->addItem(new Armour("Royal Shield", 10));
    locations[14]->addItem(new Weapon("Royal Claymore", 12));

    // The Lost Temple (3)
    locations[3]->addItem(new Weapon("Ritual Spear", 8));

    // Gerudo Training Grounds (9)
    locations[9]->addItem(new Weapon("Gerudo Spear", 10));

    // Coliseum Ruins (7)
    locations[7]->addItem(new Weapon("Cobble Crusher", 14));

    // Goron Forgeworks (26)
    locations[26]->addItem(new Weapon("Boulder Breaker", 18));

    // Zonai Sky Ruins (31)
    locations[31]->addItem(new Weapon("Magic Rod", 15));
    locations[31]->addItem(new Treasure("Star Fragment", 70));

    // Korok Forest (6)
    locations[6]->addItem(new Treasure("Korok Seed", 150));
}

Location* Gameworld::getStartLocation() {
    return locations[19]; // Lookout Landing
}
