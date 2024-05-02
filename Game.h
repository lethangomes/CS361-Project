#pragma once
#include <iostream>
#include <string>
#include <zmq.hpp>
#include "room.h"
#include "messageParser.h"
#include "ports.h"
#include "commands.h"

zmq::message_t generateMap(zmq::socket_t & socket, std::string);

class Game{
    private:
        int playerX;
        int playerY;
        int width;
        int height;
        int numRooms;
        Room** rooms;

    public:
        Game(int, int, int);
        Game(Message);
        ~Game();
        void setRoom(Room newRoom, int, int);
        void fullPrint(bool);
        std::string mapPrintString(bool);
        std::string getRoomDescription();
        void updateVisibleRooms();
        void checkAvailableCommands(Message&);
        void moveUp();
        void moveDown();
        void moveLeft();
        void moveRight();
};


//destructor
Game::~Game()
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

//updates what rooms are visible based on player position
void Game::updateVisibleRooms()
{
    //above player
    if(playerX + 1 < width)
    {
        rooms[playerX + 1][playerY].setVisible(true);
    }

    //right of player
    if(playerY + 1 < height)
    {
        rooms[playerX][playerY + 1].setVisible(true);
    }

    //below player
    if(playerX - 1 >= 0)
    {
        rooms[playerX - 1][playerY].setVisible(true);
    }

    //left of player
    if(playerY - 1 >= 0)
    {
        rooms[playerX][playerY - 1].setVisible(true);
    }

    rooms[playerX][playerY].setRevealed(true);
}
   
//sets a specified room to a new value
void Game::setRoom(Room newRoom, int x, int y)
{
    rooms[x][y] = newRoom;
}

//prints the map to console
void Game::fullPrint(bool printInvisible)
{
    for(int i = 0; i < width; i++)
    {
        //print rooms
        for(int j = 0; j < height; j++)
        {
            int type = rooms[i][j].getType();
            if(type == CLOSED || (!printInvisible && !rooms[i][j].getVisible()))
            {
                //room is closed or not visible
                std::cout << "    ";
            }
            else
            {
                if(!printInvisible && !rooms[i][j].getRevealed())
                {
                    //room is visible but has not been revealed
                    std::cout << " ? ";
                }
                else
                {
                    //check if room has player in it
                    if(i == playerX && j == playerY) std::cout << " * ";
                    else std::cout << " " << rooms[i][j].getTypeChar() << " ";
                }

                //print horizontal connector if necesary
                if(j != height - 1 && rooms[i][j+1].getType() != CLOSED && (printInvisible || rooms[i][j+1].getVisible())) std::cout << "-";
                else std::cout << " ";
            }
            
        }
        std::cout << std::endl;

        //print vertical connectors
        for(int j = 0; j < height; j++)
        {
            int type = rooms[i][j].getType();
            std::cout << " ";
            if(i == width - 1 || type == CLOSED)
            {
                //one of the rooms is closed, no connector is needed
                std::cout << "   ";
            }
            else
            {
                //check that the room above and below are not closed and are visible. If they are, print connector
                if(rooms[i+1][j].getType() != CLOSED && (printInvisible || (rooms[i+1][j].getVisible() && rooms[i][j].getVisible()))) std::cout << "|  ";
                else std::cout << "   ";
            }
            //std::cout << rooms[i][j].getType() << "\t";
        }
        std::cout << std::endl;
    }
}


//converts the map to a printable string
std::string Game::mapPrintString(bool printInvisible)
{
    std::string mapString = "";
    for(int i = 0; i < width; i++)
    {
        //print rooms
        for(int j = 0; j < height; j++)
        {
            int type = rooms[i][j].getType();
            if(type == CLOSED || (!printInvisible && !rooms[i][j].getVisible()))
            {
                //room is closed or not visible, do not print it
                mapString += "    ";
            }
            else
            {
                if(!printInvisible && !rooms[i][j].getRevealed())
                {
                    //room is visible, but not revealed
                    mapString += " ? ";
                }
                else
                {
                    //room is visible and has been revealed
                    if(i == playerX && j == playerY) mapString = mapString +  " * ";
                    else mapString = mapString +  " " + rooms[i][j].getTypeChar() + " ";
                }

                //print horizontal connector if necesary
                if(j != height - 1 && rooms[i][j+1].getType() != CLOSED && (printInvisible || rooms[i][j+1].getVisible())) mapString += "-";
                else mapString += " ";
            }
            
        }
        mapString += '\n';

        //print vertical connectors
        for(int j = 0; j < height; j++)
        {
            int type = rooms[i][j].getType();
            mapString += " ";
            if(i == width - 1 || type == CLOSED)
            {
                //the room we are currently looking at is closed, so it can't be connected to anything
                mapString += "   ";
            }
            else
            {
                //make sure room above and below are both not closed and are visible before printing connector
                if(rooms[i+1][j].getType() != CLOSED && (printInvisible || (rooms[i+1][j].getVisible() && rooms[i][j].getVisible()))) mapString += "|  ";
                else mapString += "   ";
            }
            //std::cout << rooms[i][j].getType() << "\t";
        }
        mapString += '\n';
    }

    return mapString;
}

//moves the player north
void Game::moveUp()
{
    playerX -= 1;
    updateVisibleRooms();
}

//moves the player south
void Game::moveDown()
{
    
    playerX += 1;
    updateVisibleRooms();
}

//moves the player west
void Game::moveLeft()
{
    playerY -= 1;
    updateVisibleRooms();
}

//moves the player east
void Game::moveRight()
{
    playerY += 1;
    updateVisibleRooms();
}

//checks what commands the player can use currently
void Game::checkAvailableCommands(Message& message)
{
    //create list to store whether or not 
    bool commands[NUM_COMMANDS];

    //commands that are always available
    commands[HELP] = true;
    commands[QUIT] = true;

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