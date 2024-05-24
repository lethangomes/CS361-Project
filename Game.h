#pragma once
#include <iostream>
#include <string>
#include <zmq.hpp>
#include "room.h"
#include "messageParser.h"
#include "ports.h"
#include "commands.h"

class Game{
    private:
        int playerX;
        int playerY;
        int width;
        int height;
        int numRooms;
        bool dead = false;
        Room** rooms;

        //helper functions
        void roomsPrintString(bool, std::string&, int);
        void connectorsPrintString(bool, std::string &, int);

    public:
        Game(int, int, int);
        Game(Message);
        ~Game();
        void setRoom(Room newRoom, int, int);
        void fullPrint(bool);
        std::string mapPrintString(bool);
        std::string getRoomDescription();
        void checkAvailableCommands(Message&);
        void updateMap(Message&);
        void freeMap();
        void moveUp();
        void moveDown();
        void moveLeft();
        void moveRight();
        Room** getMap();
        int getWidth();
        int getHeight();
        int getNumRooms();
        int getPlayerX();
        int getPlayerY();
        void restart(zmq::socket_t & socket);
};


//destructor
Game::~Game()
{
    freeMap();
}

void Game::freeMap()
{
    for(int i = 0; i < width; i++)
    {
        free(rooms[i]);
    }
    free(rooms);
}

//construct object from message
Game::Game(Message message)
{
    rooms = message.makeMap(width, height, numRooms);
    playerX = width/2;
    playerY = height/2;
}

//constructor
Game::Game(int width, int height, int numRooms) : width(width), height(height), numRooms(numRooms)
{
    //create rooms
    rooms = new Room * [width];
    for(int i = 0; i < width; i++)
    {
        rooms[i] = new Room[height];
    }

    playerX = width/2;
    playerY = height/2;
}

   
//sets a specified room to a new value
void Game::setRoom(Room newRoom, int x, int y)
{
    rooms[x][y] = newRoom;
}

//converts the map to a printable string. If printInvisible is true it will treat the whole map as visible
std::string Game::mapPrintString(bool printInvisible)
{
    std::string mapString = "";
    for(int i = 0; i < width; i++)
    {
        roomsPrintString(printInvisible, mapString, i);
        connectorsPrintString(printInvisible, mapString, i);
    }

    return mapString;
}

//helper function for mapPrintString. Prints one row of rooms
void Game::roomsPrintString(bool printInvisible, std::string &mapString, int row)
{
    for(int j = 0; j < height; j++)
    {
        //print room
        if(row == playerX && j == playerY) mapString = mapString +  " * ";
        else mapString = mapString +  " " + rooms[row][j].getTypeChar(!printInvisible) + " ";

        //print connector
        if(j != height - 1 && rooms[row][j+1].getType() != CLOSED && rooms[row][j].getType() != CLOSED &&
            (printInvisible || (rooms[row][j+1].getVisible() && rooms[row][j].getVisible()))) mapString += "-";
        else mapString += " ";
        
    }
    mapString += '\n';
}

//helper function for mapPrintString. Prints one row of vertical connectors
void Game::connectorsPrintString(bool printInvisible, std::string &mapString, int row)
{
    //print vertical connectors
    for(int j = 0; j < height; j++)
    {
        int type = rooms[row][j].getType();
        mapString += " ";
        if(row == width - 1 || type == CLOSED || (!printInvisible && !rooms[row][j].getVisible())) mapString += "   "; 
        else
        {
            //make sure room above is not closed and are visible before printing connector
            if(rooms[row+1][j].getType() != CLOSED && 
                (printInvisible || rooms[row+1][j].getVisible())) mapString += "|  ";
            else mapString += "   ";
        }
    }
    mapString += '\n';
}

//moves the player north
void Game::moveUp()
{
    playerX -= 1;
    //updateVisibleRooms();
}

//moves the player south
void Game::moveDown()
{
    
    playerX += 1;
    //updateVisibleRooms();
}

//moves the player west
void Game::moveLeft()
{
    playerY -= 1;
    //updateVisibleRooms();
}

//moves the player east
void Game::moveRight()
{
    playerY += 1;
    //updateVisibleRooms();
}

//checks what commands the player can use currently
void Game::checkAvailableCommands(Message& message)
{
    bool commands[NUM_COMMANDS];

    //commands that are always available
    commands[HELP] = true;
    commands[QUIT] = true;
    commands[RESTART] = true;
    commands[SAVE] = true;
    commands[LOAD] = true;

    //move north
    commands[MOVE_NORTH] = playerX != 0 && rooms[playerX - 1][playerY].getType() != CLOSED;

    //move south
    commands[MOVE_SOUTH] = playerX != width -1 && rooms[playerX + 1][playerY].getType() != CLOSED;

    //move west
    commands[MOVE_WEST] = playerY != 0 && rooms[playerX][playerY - 1].getType() != CLOSED;
    
    //move east
    commands[MOVE_EAST] = playerY != height-1 && rooms[playerX][playerY + 1].getType() != CLOSED;
    
    for(int i = 0; i < NUM_COMMANDS; i++)
    {
        message.addData("command" + std::to_string(i), std::to_string(commands[i]));
    }
}

//returns description of a room.
std::string Game::getRoomDescription()
{
    return rooms[playerX][playerY].getDescription();
}

Room** Game::getMap()
{
    return rooms;
}

int Game::getWidth()
{
    return width;
}

int Game::getHeight()
{
    return height;
}

int Game::getNumRooms()
{
    return numRooms;
}

//updates maps
void Game::updateMap(Message & message)
{
    for(int i = 0; i < message.getInt("numUpdates"); i++)
    {
        Room currentRoom(message[std::to_string(i)]);
        rooms[currentRoom.getX()][currentRoom.getY()] = currentRoom;
    }
}

int Game::getPlayerX()
{
    return playerX;
}

int Game::getPlayerY()
{
    return playerY;
}

void Game::restart(zmq::socket_t & socket)
{
    zmq::message_t response;
    socket.send(zmq::buffer(std::string("regen")), zmq::send_flags::none);
    socket.recv(response, zmq::recv_flags::none);
    std::cout << response.to_string() << std::endl;

    Message message(response.to_string());
    rooms = message.makeMap(width, height, numRooms);
    playerX = width/2;
    playerY = height/2;
}

zmq::message_t generateMap(zmq::socket_t & socket, std::string);
int processCommand(int commandNum, Game &game, zmq::socket_t & updater_socket, zmq::socket_t & generator_socket);
int gameLoop(zmq::socket_t &UI_socket, zmq::socket_t &updater_socket, Game &game, Message &UIdata, zmq::socket_t & generator_socket);
zmq::message_t regenerateMap(zmq::socket_t & socket);
void movementUpdate(Game & game, zmq::socket_t & updater_socket);