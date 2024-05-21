#pragma once
#include <iostream>
#include <string>
#include <zmq.hpp>
#include "messageParser.h"
#include "room.h"
#include "settings.h"

std::vector<std::string> splitMessage(std::string message, char delim);
void printHelp(std::vector<bool>);
void printHelp(std::string);
void printHelpInfo(int, std::string, std::string);
void changeSettings(std::string&, std::string&, std::string&, std::string[NUMROOMTYPES]);
bool validateInput(std::string input);
void settingsPage(Message & generationSettings, zmq::socket_t & socket, std::unordered_map<std::string, Setting>&);
void updateSetting(std::string name, std::string & setting, int defaultVal, int recMin, int recMax, int min, int max, std::string notes, std::string description);
void setDefaultVals(std::string & width, std::string & height, std::string & numRooms, std::string numRoomTypes[NUMROOMTYPES - SPAWN - 1]);
void initializeSettings(std::unordered_map<std::string, Setting>&);