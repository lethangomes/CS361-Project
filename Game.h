#pragma once
#include <iostream>
#include <string>
#include <zmq.hpp>
#include "room.h"
#include "messageParser.h"
#include "ports.h"

zmq::message_t generateMap(zmq::socket_t & socket, std::string);
void fullPrint(bool printInvisible, int width, int height, Room ** rooms);
std::string mapPrintString(bool printInvisible, int width, int height, Room ** rooms);