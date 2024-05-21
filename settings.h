#pragma once
#include <iostream>
#include <unordered_map>

struct Setting{
    int value;
    int min;
    int max;
    int defaultVal;
    int recMin;
    int recMax;
    std::string notes;
    std::string description;
};

#define DEFAULT_WIDTH 10
#define DEFAULT_HEIGHT 10
#define DEFAULT_NUMROOMS 25
#define DEFAULT_NUM_GOLD 1
#define DEFAULT_NUM_MONSTER 3
#define DEFAULT_NUM_TRAP 4

#define NUM_SETTINGS 6
#define WIDTH 0
#define HEIGHT 1
#define NUM_ROOMS 2
#define NUM_GOLD 3
#define NUM_MONSTERS 4
#define NUM_TRAP 5

static const std::string SETTING_NAMES[NUM_SETTINGS] = {"width", "height", "numRooms", "numGold", "numMonster", "numTrap"};
static const int SETTING_DEFAULTS[NUM_SETTINGS] = {DEFAULT_WIDTH, DEFAULT_HEIGHT, DEFAULT_NUMROOMS, DEFAULT_NUM_GOLD, DEFAULT_NUM_MONSTER, DEFAULT_NUM_TRAP};
static const int SETTING_MAXES[NUM_SETTINGS] = {20, 20, 400, 100, 100, 100};
static const int SETTING_MINS[NUM_SETTINGS] = {3, 4, 5, 1, 1, 1};
static const int SETTING_REC_MAXES[NUM_SETTINGS] = {15, 15, 50, 5, 5, 5};
static const int SETTING_REC_MINS[NUM_SETTINGS] = {5, 5, 10, 1, 1, 1};
static const std::string SETTING_NOTES[NUM_SETTINGS] = {
    "width - The maximum width of the dungeon",
    "height - The maximum height of the dungeon",
    "numRooms - The total number of rooms that will be generated",
    "numGold - The number of rooms that will spawn with gold",
    "numMonster - The number of rooms that will spawn with monsters in them",
    "numTrap - The number of rooms that will spawn with traps"
};

static const std::string SETTING_DESCRIPTIONS[NUM_SETTINGS] = {
    "the dungeon generates in a rectangular grid. This setting changes how large that grid is horizontally. If the grid doesn't have enough space for the number of rooms you generate some of those rooms won't generate",
    "the dungeon generates in a rectangular grid. This setting changes how large that grid is vertically. If the grid doesn't have enough space for the number of rooms you generate some of those rooms won't generate",
    "the dungeon will only generated up to width * height rooms, even if this setting is set to a larger value. Additionally, if you try to generate more special rooms(gold, traps, and monsters) than there are total rooms, some of the special rooms will not generate",
    "Gold rooms are the win condition of this game. Your goal is to find as many of them as you can and leave without dying",
    "Monsters are enemies that roam the dungeon. Entering a room with one will instantly kill you. They can be killed, which will remove them from the map",
    "Trapped rooms are rooms that cause a random negative effect for the player"
};