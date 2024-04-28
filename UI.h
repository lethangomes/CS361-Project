#pragma once
#include <iostream>
#include <string>
#include <zmq.hpp>
#include "room.h"
#include "messageParser.h"

zmq::message_t generateMap(zmq::socket_t & socket);
void fullPrint(bool printInvisible, int width, int height, Room ** rooms);