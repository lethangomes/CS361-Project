#pragma once
#define NUM_COMMANDS 9
#define QUIT 0
#define MOVE_NORTH 1
#define MOVE_SOUTH 2
#define MOVE_EAST 3
#define MOVE_WEST 4
#define HELP 5
#define RESTART 6
#define SAVE 7
#define LOAD 8

//when adding commands, update processCommand in game.cpp, checkAvailableCommands in game.h and the arrays
//at the top of UI.cpp