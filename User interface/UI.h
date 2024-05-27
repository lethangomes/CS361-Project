#pragma once
#include <iostream>
#include <string>
#include <zmq.hpp>
#include "../messageParser.h"
#include "../room.h"
#include "settings.h"

std::vector<std::string> splitMessage(std::string message, char delim);
void printHelp(std::vector<bool>);
void printHelp(std::string);
void printHelpInfo(int, std::string, std::string);
void changeSettings(Settings & setting);
bool validateInput(std::string input);
void settingsPage(Message & generationSettings, zmq::socket_t & socket, std::unordered_map<std::string, Setting>&);
void updateSetting(Setting & setting);
void setDefaultVals(std::unordered_map<std::string, Setting> & settings);
void initializeSettings(std::unordered_map<std::string, Setting>&);
void addSettingsToMessage(Settings & settings, Message & message);
int processCommand(std::string input, std::vector<bool> commandAvailablity, Message & command);